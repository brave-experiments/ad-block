/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <algorithm>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include "./ABPFilterParser.h"

using std::cout;
using std::endl;
using std::string;

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

void writeFile(const char *filename, const char *buffer, int length) {
  std::ofstream outFile(filename, std::ios::out | std::ios::binary);
  if (outFile) {
    outFile.write(buffer, length);
    outFile.close();
    return;
  }
  throw(errno);
}

int checkForParser(ABPFilterParser *pParser, const char *outputPath,
    const std::vector<std::string> &urlsToCheck) {
  ABPFilterParser &parser = *pParser;

  // This is the site who's URLs are being checked, not the domain of the
  // URL being checked.
  const char *currentPageDomain = "slashdot.org";


  // Do the checks
  std::for_each(urlsToCheck.begin(), urlsToCheck.end(),
      [&parser, currentPageDomain](std::string const &urlToCheck) {
    if (parser.matches(urlToCheck.c_str(),
          FONoFilterOption, currentPageDomain)) {
      cout << urlToCheck << ": You should block this URL!" << endl;
    } else {
      cout << urlToCheck << ": You should NOT block this URL!" << endl;
    }
  });

  int size;
  // This buffer is allocate on the heap, you must call delete[] when
  // you're done using it.
  char *buffer = parser.serialize(&size);
  writeFile(outputPath, buffer, size);

  ABPFilterParser parser2;
  // Deserialize uses the buffer directly for subsequent matches, do not free
  // until all matches are done.
  parser2.deserialize(buffer);
  // Prints the same as parser.matches would
  std::for_each(urlsToCheck.begin(), urlsToCheck.end(),
      [&parser2, currentPageDomain](std::string const &urlToCheck) {
    if (parser2.matches(urlToCheck.c_str(),
          FONoFilterOption, currentPageDomain)) {
      cout << urlToCheck << ": You should block this URL!" << endl;
    } else {
      cout << urlToCheck << ": You should NOT block this URL!" << endl;
    }
  });
  delete[] buffer;
  return 0;
}


int main(int argc, char**argv) {
  std::string && easyListTxt = getFileContents("./test/data/easylist.txt");
  std::string && ublockUnblockTxt =
    getFileContents("./test/data/ublock-unbreak.txt");
  std::string && braveUnblockTxt =
    getFileContents("./test/data/brave-unbreak.txt");
  std::string && spam404MainBlacklistTxt =
    getFileContents("./test/data/spam404-main-blacklist.txt");
  std::string && disconnectSimpleMalwareTxt =
    getFileContents("./test/data/disconnect-simple-malware.txt");

  // Parse filter lists for adblock
  ABPFilterParser adBlockParser;
  adBlockParser.parse(easyListTxt.c_str());
  adBlockParser.parse(ublockUnblockTxt.c_str());
  adBlockParser.parse(braveUnblockTxt.c_str());
  std::vector<std::string> checkVector;
  checkVector.push_back(
      "http://pagead2.googlesyndication.com/pagead/show_ads.js");
  checkVector.push_back(
      "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html");
  checkVector.push_back(
      "http://www.googletagservices.com/tag/js/gpt_mobile.js");
  checkVector.push_back("http://www.brianbondy.com");
  checkForParser(&adBlockParser, "./ABPFilterParserData.dat", checkVector);

  // Parse filter lists for malware
  ABPFilterParser malwareParser;
  malwareParser.parse(spam404MainBlacklistTxt.c_str());
  malwareParser.parse(disconnectSimpleMalwareTxt.c_str());
  std::vector<std::string> checkVector2;
  checkVector2.push_back("http://freexblcode.com/test");
  checkVector2.push_back("https://malware-check.disconnect.me");
  checkVector2.push_back("http://www.brianbondy.com");
  checkForParser(&malwareParser, "./SafeBrowsingData.dat", checkVector2);

  return 0;
}
