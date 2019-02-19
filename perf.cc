/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <time.h>
#include <cerrno>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include "./ad_block_client.h"
#include "./bad_fingerprint.h"

using std::string;
using std::cout;
using std::endl;

string getFileContents(const char *filename) {
  std::ifstream in(filename, std::ios::in);
  if (in) {
    std::ostringstream contents;
    contents << in.rdbuf();
    in.close();
    return(contents.str());
  }
  throw(errno);
}

void doSiteList(AdBlockClient *pClient, bool outputPerf) {
  AdBlockClient &client = *pClient;
  // Assumes the following format in the site list:
  // Each record consists of 3 lines with no special identifiers:
  // 1. site (frame) url
  // 2. request url - the url that is being matched within the frame context
  // 3. request type (document, stylesheet, image, script, etc)
  std::string && siteList = getFileContents("./test/data/sitelist-ref-type.txt");
  std::stringstream ss(siteList);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;

  std::vector<std::string> lines(begin, end);

  // Build up a tuple of <site_url, request_url, filter_option>
  std::vector<std::tuple<std::string, std::string, FilterOption> > sites;

  for (auto i = lines.begin(); i != lines.end(); std::advance(i, 3)) {

    const std::string& frameUrl = *i;
    // cout << "Parsed item " << frameUrl << endl;
    const std::string& url = *(std::next(i, 1));
    const std::string& cpt = *(std::next(i, 2));

    FilterOption fo = FONoFilterOption;

    if (cpt == "document" ) {
      fo = FOSubdocument;
    } else if (cpt == "stylesheet" ) {
      fo = FOStylesheet;
    } else if (cpt == "image" ) {
      fo = FOImage;
    } else if (cpt == "media" ) {
      fo = FOMedia;
    } else if (cpt == "font" ) {
      fo = FOFont;
    } else if (cpt == "script" ) {
      fo = FOScript;
    } else if (cpt == "xhr" ) {
      fo = FOXmlHttpRequest;
    } else if (cpt == "websocket" ) {
      fo = FOWebsocket;
    } else {
      fo = FOOther;
    }

    sites.push_back(std::make_tuple(frameUrl, url, fo));
  }

  int numBlocks = 0;
  int numSkips = 0;
  const clock_t beginTime = clock();
  std::for_each(sites.begin(), sites.end(), [&client, &numBlocks,
    &numSkips](std::tuple<std::string, std::string, FilterOption> const &urlToCheck) {
    if (client.matches(std::get<1>(urlToCheck).c_str(),
      std::get<2>(urlToCheck),
      std::get<0>(urlToCheck).c_str())) {
      ++numBlocks;
    } else {
      ++numSkips;
    }
  });
  if (outputPerf) {
    cout << "Time: " << float(clock() - beginTime)
      / CLOCKS_PER_SEC << "s" << endl;
    cout << "num blocks: " << numBlocks << ", num skips: " << numSkips << endl;
    cout << "False Positives: " << client.numFalsePositives
      << ", exception false positives: "
      << client.numExceptionFalsePositives << endl;
    cout << "Bloom filter saves: " << client.numBloomFilterSaves
      << ", exception bloom filter saves: "
      << client.numExceptionBloomFilterSaves << endl;
  }
}

int main(int argc, char**argv) {
  std::string && easyListTxt =
    getFileContents("./test/data/easylist.txt");
  std::string && easyPrivacyTxt =
    getFileContents("./test/data/easyprivacy.txt");
  std::string && braveUnblockTxt =
    getFileContents("./test/data/brave-unbreak.txt");
  std::string && ublockUnblockTxt =
    getFileContents("./test/data/ublock-unbreak.txt");
  std::string && spam404MainBlacklistTxt =
    getFileContents("./test/data/spam404-main-blacklist.txt");
  std::string && disconnectSimpleMalwareTxt =
    getFileContents("./test/data/disconnect-simple-malware.txt");

  cout << endl
    << "-------------\n"
    << "  AD BLOCK   \n"
    << "-------------\n"
    << endl;

  AdBlockClient adBlockClient;
  adBlockClient.parse(easyListTxt.c_str());
  adBlockClient.parse(easyPrivacyTxt.c_str());
  adBlockClient.parse(ublockUnblockTxt.c_str());
  adBlockClient.parse(braveUnblockTxt.c_str());
  doSiteList(&adBlockClient, true);

  cout << endl
    << "-------------\n"
    << "SAFE BROWSING\n"
    << "-------------\n"
    << endl;

  AdBlockClient safeBrowsingClient;
  safeBrowsingClient.parse(spam404MainBlacklistTxt.c_str());
  safeBrowsingClient.parse(disconnectSimpleMalwareTxt.c_str());
  doSiteList(&safeBrowsingClient, true);

  cout << endl
    << "-------------\n"
    << "generating bad fingerprints list"
    << endl;

  AdBlockClient allClient;
  allClient.enableBadFingerprintDetection();
  allClient.parse(easyListTxt.c_str());
  allClient.parse(easyPrivacyTxt.c_str());
  allClient.parse(ublockUnblockTxt.c_str());
  allClient.parse(braveUnblockTxt.c_str());
  allClient.parse(spam404MainBlacklistTxt.c_str());
  allClient.parse(disconnectSimpleMalwareTxt.c_str());
  doSiteList(&allClient, false);
  allClient.badFingerprintsHashSet->generateHeader("bad_fingerprints.h");

  return 0;
}
