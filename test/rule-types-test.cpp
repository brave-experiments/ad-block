#include "CppUnitLite/TestHarness.h"
#include "ABPFilterParser.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include "util.h"
#include "string.h"
#include <iostream>
#include <set>

using namespace std;

bool testComment( const char *rawFilter) {
  Filter filter;
  parseFilter(rawFilter, filter);
  return filter.filterType == comment;
}

TEST(ruleTypes, commentRules) {
  set<string> commentRules {
    "[Adblock Plus 2.0]",
    "! Checksum: nVIXktYXKU6M+cu+Txkhuw",
    "!/cb.php?sub$script,third-party",
    "!@@/cb.php?sub",
    "!###ADSLOT_SKYSCRAPER",
    "! *** easylist:easylist/easylist_whitelist_general_hide.txt ***",
    "   !###ADSLOT_SKYSCRAPER",
  };

  std::for_each(commentRules.begin(), commentRules.end(), [this, &result_](string const &s) {
    CHECK(testComment(s.c_str()));
  }
);

TEST(ruleTypes, elemHidingRules) {
  CHECK(true);
}

/*
let elementHidingRules = new Set([
  "   ###ADSLOT_SKYSCRAPER",
  "###ADSLOT_SKYSCRAPER",
  "@@###ADSLOT_SKYSCRAPER",
  "##.adsBox",
  "eee.se#@##adspace_top",
  "domain1.com,domain2.com#@##adwrapper",
  "edgesuitedomain.net#@##ad-unit",
  "mydomain.com#@#.ad-unit",
  "##a[href^=\"http://affiliate.sometracker.com/\"]",
]);
*/
