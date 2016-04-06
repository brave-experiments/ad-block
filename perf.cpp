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
#include "./ABPFilterParser.h"

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

void doSiteList(ABPFilterParser *pParser, bool outputPerf) {
  ABPFilterParser &parser = *pParser;
  std::string && siteList = getFileContents("./test/data/sitelist.txt");
  std::stringstream ss(siteList);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> sites(begin, end);

  // This is the site who's URLs are being checked, not the domain of
  // the URL being checked.
  const char *currentPageDomain = "brianbondy.com";

  int numBlocks = 0;
  int numSkips = 0;
  const clock_t beginTime = clock();
  std::for_each(sites.begin(), sites.end(), [&parser, currentPageDomain,
      &numBlocks, &numSkips](std::string const &urlToCheck) {
    if (parser.matches(urlToCheck.c_str(), FONoFilterOption,
          currentPageDomain)) {
      ++numBlocks;
    } else {
      ++numSkips;
    }
  });
  if (outputPerf) {
    cout << "Time: " << float(clock() - beginTime)
      / CLOCKS_PER_SEC << "s" << endl;
    cout << "num blocks: " << numBlocks << ", num skips: " << numSkips << endl;
    cout << "False Positives: " << parser.numFalsePositives
      << ", exception false positives: "
      << parser.numExceptionFalsePositives << endl;
    cout << "Bloom filter saves: " << parser.numBloomFilterSaves
      << ", exception bloom filter saves: "
      << parser.numExceptionBloomFilterSaves << endl;
  }
}

int main(int argc, char**argv) {
  std::string && easyListTxt =
    getFileContents("./test/data/easylist.txt");
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

  ABPFilterParser adBlockParser;
  adBlockParser.parse(easyListTxt.c_str());
  adBlockParser.parse(ublockUnblockTxt.c_str());
  adBlockParser.parse(braveUnblockTxt.c_str());
  doSiteList(&adBlockParser, true);

  cout << endl
    << "-------------\n"
    << "SAFE BROWSING\n"
    << "-------------\n"
    << endl;

  ABPFilterParser safeBrowsingParser;
  safeBrowsingParser.parse(spam404MainBlacklistTxt.c_str());
  safeBrowsingParser.parse(disconnectSimpleMalwareTxt.c_str());
  doSiteList(&safeBrowsingParser, true);

  cout << endl
    << "-------------\n"
    << "generating bad fingerprints list"
    << endl;

  ABPFilterParser allParser;
  allParser.enableBadFingerprintDetection();
  allParser.parse(easyListTxt.c_str());
  allParser.parse(ublockUnblockTxt.c_str());
  allParser.parse(braveUnblockTxt.c_str());
  allParser.parse(spam404MainBlacklistTxt.c_str());
  allParser.parse(disconnectSimpleMalwareTxt.c_str());
  doSiteList(&allParser, false);
  allParser.badFingerprintsHashSet->generateHeader("badFingerprints.h");

  return 0;
}
