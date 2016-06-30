/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <algorithm>
#include <iostream>
#include <set>
#include "./CppUnitLite/TestHarness.h"
#include "./CppUnitLite/Test.h"
#include "./ABPFilterParser.h"
#include "./util.h"

using std::string;
using std::endl;
using std::set;
using std::cout;

bool testFilter(const char *rawFilter, FilterType expectedFilterType,
    FilterOption expectedFilterOption,
    const char *expectedData,
    set<string> &&blocked, // NOLINT
    set<string> &&notBlocked) { // NOLINT
  Filter filter;
  parseFilter(rawFilter, &filter);

  if (filter.filterOption != expectedFilterOption) {
    cout << "Actual filter option: " << filter.filterOption
      << endl << "Expected: " << expectedFilterOption << endl;
    return false;
  }

  if (filter.filterType != expectedFilterType) {
    cout << "Actual filter type: " << filter.filterType
      << endl << "Expected: " << expectedFilterType << endl;
    return false;
  }

  if (strcmp(filter.data, expectedData)) {
    cout << "Actual filter data: " << filter.data
      << endl << "Expected: " << expectedData << endl;
    return false;
  }

  bool ret = true;
  string lastChecked;
  std::for_each(blocked.begin(), blocked.end(),
      [&filter, &ret, &lastChecked](string const &s) {
    ret = ret && filter.matches(s.c_str());
    lastChecked = s;
  });
  if (!ret) {
    cout << "Should match but did not: " << lastChecked.c_str() << endl;
    return false;
  }

  std::for_each(notBlocked.begin(), notBlocked.end(),
      [&filter, &ret, &lastChecked](string const &s) {
    ret = ret && !filter.matches(s.c_str());
    lastChecked = s;
  });
  if (!ret) {
    cout << "Should NOT match but did: " << lastChecked.c_str() << endl;
    return false;
  }

  return true;
}

TEST(parser, parseFilterMatchesFilter) {
  CHECK(testFilter("/banner/*/img",
    FTNoFilterType,
    FONoFilterOption,
    "/banner/*/img",
    {
      "http://example.com/banner/foo/img",
      "http://example.com/banner/foo/bar/img?param",
      "http://example.com/banner//img/foo",
      "http://example.com/banner//img.gif",
    }, {
      "http://example.com/banner",
      "http://example.com/banner/",
      "http://example.com/banner/img",
      "http://example.com/img/banner/",
    }));

  CHECK(testFilter("/banner/*/img^",
    FTNoFilterType,
    FONoFilterOption,
    "/banner/*/img^",
    {
      "http://example.com/banner/foo/img",
      "http://example.com/banner/foo/bar/img?param",
      "http://example.com/banner//img/foo",
    }, {
      "http://example.com/banner/img",
      "http://example.com/banner/foo/imgraph",
      "http://example.com/banner/foo/img.gif",
    }));

  CHECK(testFilter("||ads.example.com^",
    static_cast<FilterType>(FTHostAnchored | FTHostOnly),
    FONoFilterOption,
    "ads.example.com^",
    {
      "http://ads.example.com/foo.gif",
      "http://server1.ads.example.com/foo.gif",
      "https://ads.example.com:8000/",
    }, {
      "http://ads.example.com.ua/foo.gif",
      "http://example.com/redirect/http://ads.example.com/",
    }));

  CHECK(testFilter("|http://example.com/|",
    static_cast<FilterType>(FTLeftAnchored | FTRightAnchored),
    FONoFilterOption,
    "http://example.com/",
    {
      "http://example.com/"
    }, {
      "http://example.com/foo.gif",
      "http://example.info/redirect/http://example.com/",
    }));

  CHECK(testFilter("swf|",
    FTRightAnchored,
    FONoFilterOption,
    "swf",
    {
      "http://example.com/annoyingflash.swf",
    },
    {
      "http://example.com/swf/index.html"
    }));

  CHECK(testFilter("|http://baddomain.example/",
    FTLeftAnchored,
    FONoFilterOption,
    "http://baddomain.example/",
    {
     "http://baddomain.example/banner.gif",
    },
    {
      "http://gooddomain.example/analyze?http://baddomain.example",
    }));

  CHECK(testFilter("||example.com/banner.gif",
    FTHostAnchored,
    FONoFilterOption,
    "example.com/banner.gif",
    {
      "http://example.com/banner.gif",
      "https://example.com/banner.gif",
      "http://www.example.com/banner.gif",
    },
    {
      "http://badexample.com/banner.gif",
      "http://gooddomain.example/analyze?http://example.com/banner.gif",
      "http://example.com.au/banner.gif",
      "http://example.com/banner2.gif",
    }));

  CHECK(testFilter("http://example.com^",
    FTNoFilterType,
    FONoFilterOption,
    "http://example.com^",
    {
      "http://example.com/",
      "http://example.com:8000/ ",
    },
    {}));

  CHECK(testFilter("^example.com^",
    FTNoFilterType,
    FONoFilterOption,
    "^example.com^",
    {
      "http://example.com:8000/foo.bar?a=12&b=%D1%82%D0%B5%D1%81%D1%82",
    },
    {}));
  CHECK(testFilter("^%D1%82%D0%B5%D1%81%D1%82^",
    FTNoFilterType,
    FONoFilterOption,
    "^%D1%82%D0%B5%D1%81%D1%82^",
    {
      "http://example.com:8000/foo.bar?a=12&b=%D1%82%D0%B5%D1%81%D1%82",
    },
    {
      "http://example.com:8000/foo.bar?a=12&b%D1%82%D0%B5%D1%81%D1%823",
    }));
  CHECK(testFilter("^foo.bar^",
    FTNoFilterType,
    FONoFilterOption,
    "^foo.bar^",
    {
      "http://example.com:8000/foo.bar?a=12&b=%D1%82%D0%B5%D1%81%D1%82"
    },
    {}));
#ifdef ENABLE_REGEX
  CHECK(testFilter("/banner[0-9]+/",
    FTRegex,
    FONoFilterOption,
    "banner[0-9]+",
    {
      "banner123",
      "testbanner1"
    },
    {
      "banners",
      "banners123"
    }));
#endif
  CHECK(testFilter(
    "||static.tumblr.com/dhqhfum/WgAn39721/cfh_header_banner_v2.jpg",
    FTHostAnchored,
    FONoFilterOption,
    "static.tumblr.com/dhqhfum/WgAn39721/cfh_header_banner_v2.jpg",
    {
      "http://static.tumblr.com/dhqhfum/WgAn39721/cfh_header_banner_v2.jpg"
    },
    {}));

  CHECK(testFilter("||googlesyndication.com/safeframe/$third-party",
    FTHostAnchored,
    FOThirdParty,
    "googlesyndication.com/safeframe/",
    {
      "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html"
      "#xpc=sf-gdn-exp-2&p=http%3A//slashdot.org;",
    },
    {}));
  CHECK(testFilter("||googlesyndication.com/safeframe/$third-party,script",
    FTHostAnchored,
    static_cast<FilterOption>(FOThirdParty|FOScript),
    "googlesyndication.com/safeframe/",
    {
      "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html"
      "#xpc=sf-gdn-exp-2&p=http%3A//slashdot.org;",
    },
    {}));
}

bool checkMatch(const char *rules,
    set<string> &&blocked, // NOLINT
    set<string> &&notBlocked) { // NOLINT
  ABPFilterParser parsers[2];
  char * buffer = nullptr;
  for (int i = 0; i < 2; i++) {
    ABPFilterParser &parser = parsers[i];
    if (i == 0) {
      parser.parse(rules);
      int size;
      buffer = parsers[0].serialize(&size);
    } else {
      parser.deserialize(buffer);
    }

    bool ret = true;
    string lastChecked;
    std::for_each(blocked.begin(), blocked.end(),
        [&parser, &lastChecked, &ret](string const &s) {
      ret = ret && parser.matches(s.c_str());
      lastChecked = s;
    });
    if (!ret) {
      cout << "Should match but did not: " << lastChecked.c_str() << endl;
      delete[] buffer;
      return false;
    }

    std::for_each(notBlocked.begin(), notBlocked.end(),
        [&parser, &ret, &lastChecked](string const &s) {
      ret = ret && !parser.matches(s.c_str());
      lastChecked = s;
    });
    if (!ret) {
      cout << "Should NOT match but did: " << lastChecked.c_str() << endl;
      delete[] buffer;
      return false;
    }
  }
  delete[] buffer;
  return true;
}

TEST(parser, exceptionRules) {
  CHECK(checkMatch("adv\n"
                   "@@advice.",
    {
      "http://example.com/advert.html"
    }, {
      "http://example.com/advice.html",
    }));

  CHECK(checkMatch("@@advice.\n"
                   "adv",
    {
      "http://example.com/advert.html"
    }, {
      "http://example.com/advice.html"
    }));
  CHECK(checkMatch("@@|http://example.com\n"
                   "@@advice.\n"
                   "adv\n"
                   "!foo",
    {
      "http://examples.com/advert.html",
    }, {
      "http://example.com/advice.html",
      "http://example.com/advert.html",
      "http://examples.com/advice.html",
      "http://examples.com/#!foo",
    }));
  CHECK(checkMatch("/ads/freewheel/*\n"
                   "@@||turner.com^*/ads/freewheel/*/"
                     "AdManager.js$domain=cnn.com",
    {
    }, {
      "http://z.cdn.turner.com/xslo/cvp/ads/freewheel/js/0/AdManager.js",
    }));
}

struct OptionRuleData {
  OptionRuleData(const char *testUrl, FilterOption context,
      const char *contextDomain, bool shouldBlock) {
    this->testUrl = testUrl;
    this->context = context;
    this->contextDomain = contextDomain;
    this->shouldBlock = shouldBlock;
  }

  bool operator<(const OptionRuleData& rhs) const {
    return this->testUrl - rhs.testUrl < 0;
  }

  const char *testUrl;
  FilterOption context;
  const char *contextDomain;
  bool shouldBlock;
};

bool checkOptionRule(const char *rules,
    set<OptionRuleData> &&optionTests) { // NOLINT
  ABPFilterParser parser;
  parser.parse(rules);

  bool fail = false;
  std::for_each(optionTests.begin(), optionTests.end(),
      [&parser, &fail](OptionRuleData const &data) {
    bool matches = parser.matches(data.testUrl,
        data.context, data.contextDomain);
    if (matches != data.shouldBlock) {
      cout << "Expected to block: " << data.shouldBlock
        << endl << "Actual blocks: " << matches << endl;
      fail = true;
      return;
    }
  });
  if (fail) {
    return false;
  }

  return true;
}

// Option rules
TEST(parser, optionRules) {
  CHECK(checkOptionRule("||example.com",
    {
      OptionRuleData("http://example.com",
        FOThirdParty, nullptr, true),
      OptionRuleData("http://example2.com", FOThirdParty, nullptr, false),
      OptionRuleData("http://example.com", FONotThirdParty, nullptr, true),
    }));

  CHECK(checkOptionRule("||example.com^$third-party",
    {
      OptionRuleData("http://example.com", FOScript, "brianbondy.com", true),
      OptionRuleData("http://example.com", FOScript, "example.com", false),
      OptionRuleData("http://ad.example.com", FOScript, "brianbondy.com", true),
      OptionRuleData("http://ad.example.com", FOScript, "example.com", false),
      OptionRuleData("http://example2.com", FOScript, "brianbondy.com", false),
      OptionRuleData("http://example2.com", FOScript, "example.com", false),
      OptionRuleData("http://example.com.au", FOScript,
          "brianbondy.com", false),
      OptionRuleData("http://example.com.au", FOScript, "example.com", false),
    }));

  CHECK(checkOptionRule("||example.com^$third-party,~script",
    {
      OptionRuleData("http://example.com",
          static_cast<FilterOption>(FOThirdParty | FOScript), nullptr, false),
      OptionRuleData("http://example.com", FOOther, nullptr, true),
      OptionRuleData("http://example2.com",
          static_cast<FilterOption>(FOThirdParty | FOOther), nullptr, false),
      OptionRuleData("http://example.com",
          static_cast<FilterOption>(FONotThirdParty | FOOther), nullptr, false),
    }));

  CHECK(checkOptionRule("adv$domain=example.com|example.net",
    {
      OptionRuleData("http://example.net/adv",
          FONoFilterOption, "example.net", true),
      OptionRuleData("http://somewebsite.com/adv",
          FONoFilterOption, "example.com", true),
      OptionRuleData("http://www.example.net/adv",
          FONoFilterOption, "www.example.net", true),
      OptionRuleData("http://my.subdomain.example.com/adv",
          FONoFilterOption, "my.subdomain.example.com", true),
      OptionRuleData("http://my.subdomain.example.com/adv",
          FONoFilterOption, "my.subdomain.example.com", true),
      OptionRuleData("http://example.com/adv",
          FONoFilterOption, "badexample.com", false),
      OptionRuleData("http://example.com/adv",
          FONoFilterOption, "otherdomain.net", false),
      OptionRuleData("http://example.net/ad",
          FONoFilterOption, "example.net", false),
    }));

  CHECK(checkOptionRule("adv$domain=example.com|~foo.example.com",
    {
      OptionRuleData("http://example.net/adv",
          FONoFilterOption, "example.com", true),
      OptionRuleData("http://example.net/adv",
          FONoFilterOption, "foo.example.com", false),
      OptionRuleData("http://example.net/adv",
          FONoFilterOption, "www.foo.example.com", false),
    }));

  CHECK(checkOptionRule("adv$domain=~example.com|foo.example.com",
    {
      OptionRuleData("http://example.net/adv",
          FONoFilterOption, "example.com", false),
      OptionRuleData("http://example.net/adv",
          FONoFilterOption, "foo.example.com", true),
      OptionRuleData("http://example.net/adv",
          FONoFilterOption, "www.foo.example.com", true),
    }));

  CHECK(checkOptionRule("adv$domain=~example.com",
    {
      OptionRuleData("http://example.net/adv",
          FONoFilterOption, "otherdomain.com", true),
      OptionRuleData("http://somewebsite.com/adv",
          FONoFilterOption, "example.com", false),
    }));

  CHECK(checkOptionRule("adv$domain=~example.com|~example.net",
    {
      OptionRuleData("http://example.net/adv",
          FONoFilterOption, "example.net", false),
      OptionRuleData("http://somewebsite.com/adv",
          FONoFilterOption, "example.com", false),
      OptionRuleData("http://www.example.net/adv",
          FONoFilterOption, "www.example.net", false),
      OptionRuleData("http://my.subdomain.example.com/adv",
          FONoFilterOption, "my.subdomain.example.com", false),
      OptionRuleData("http://example.com/adv",
          FONoFilterOption, "badexample.com", true),
      OptionRuleData("http://example.com/adv",
          FONoFilterOption, "otherdomain.net", true),
      OptionRuleData("http://example.net/ad",
          FONoFilterOption, "example.net", false),
    }));

  CHECK(checkOptionRule("adv$domain=example.com|~example.net",
    {
      OptionRuleData("http://example.net/adv",
          FONoFilterOption, "example.net", false),
      OptionRuleData("http://somewebsite.com/adv",
          FONoFilterOption, "example.com", true),
      OptionRuleData("http://www.example.net/adv",
          FONoFilterOption, "www.example.net", false),
      OptionRuleData("http://my.subdomain.example.com/adv",
          FONoFilterOption, "my.subdomain.example.com", true),
      OptionRuleData("http://example.com/adv",
          FONoFilterOption, "badexample.com", false),
      OptionRuleData("http://example.com/adv",
          FONoFilterOption, "otherdomain.net", false),
      OptionRuleData("http://example.net/ad",
          FONoFilterOption, "example.net", false),
    }));

  CHECK(checkOptionRule(
        "adv$domain=example.com|~foo.example.com,script",
    {
      OptionRuleData("http://example.net/adv",
          FOScript, "example.com", true),
      OptionRuleData("http://example.net/adv",
          FOScript, "foo.example.com", false),
      OptionRuleData("http://example.net/adv",
          FOScript, "www.foo.example.com", false),
      OptionRuleData("http://example.net/adv",
          FOOther, "example.com", false),
      OptionRuleData("http://example.net/adv",
          FOOther, "foo.example.com", false),
      OptionRuleData("http://example.net/adv",
          FOOther, "www.foo.example.com", false),
    }));

  CHECK(checkOptionRule("adv\n"
                        "@@advice.$~script",
    {
      OptionRuleData("http://example.com/advice.html",
          FOOther, nullptr, false),
      OptionRuleData("http://example.com/advice.html",
          FOScript, nullptr, true),
      OptionRuleData("http://example.com/advert.html",
          FOOther, nullptr, true),
      OptionRuleData("http://example.com/advert.html",
          FOScript, nullptr, true),
    }));

  // Single matching context domain to domain list
  CHECK(checkOptionRule(
        "||mzstatic.com^$image,object-subrequest,domain=dailymotion.com",
    {
      OptionRuleData("http://www.dailymotion.com",
          FONoFilterOption, "dailymotion.com", false),
    }));

  // Third party flags work correctly
  CHECK(checkOptionRule(
        "||s1.wp.com^$subdocument,third-party",
    {
      OptionRuleData("http://s1.wp.com/_static",
          FOScript, "windsorstar.com", false),
    }));

  // Third party flags work correctly
  CHECK(checkOptionRule(
        "/scripts/ad.",
    {
      OptionRuleData("http://a.fsdn.com/sd/js/scripts/ad.js?release_20160112",
          FOScript, "slashdot.org", true),
    }));
}

struct ListCounts {
  size_t filters;
  size_t htmlRules;
  size_t exceptions;
};

ListCounts easyList = { 20370, 29374, 4209 };
ListCounts ublockUnbreak = { 3, 6, 72 };
ListCounts braveUnbreak = { 1, 0, 2 };
ListCounts disconnectSimpleMalware = { 3009, 0, 0 };
ListCounts spam404MainBlacklist = { 5294, 169, 0 };


// Should parse EasyList without failing
TEST(parser, parse_easylist) {
  string && fileContents = // NOLINT
    getFileContents("./test/data/easylist.txt");
  ABPFilterParser parser;
  parser.parse(fileContents.c_str());

  CHECK(compareNums(parser.numFilters +
          parser.numNoFingerprintFilters +
          parser.hostAnchoredHashSet->size(),
        easyList.filters));
  CHECK(compareNums(parser.numHtmlRuleFilters, easyList.htmlRules));
  CHECK(compareNums(parser.numExceptionFilters +
          parser.numNoFingerprintExceptionFilters +
          parser.hostAnchoredExceptionHashSet->size(),
        easyList.exceptions));
}

// Should parse ublock-unbreak list without failing
TEST(parser, parse_ublock_unbreak) {
  string && fileContents = // NOLINT
    getFileContents("./test/data/ublock-unbreak.txt");
  ABPFilterParser parser;
  parser.parse(fileContents.c_str());

  CHECK(compareNums(parser.numFilters +
         parser.numNoFingerprintFilters +
          parser.hostAnchoredHashSet->size(),
        ublockUnbreak.filters));
  CHECK(compareNums(parser.numHtmlRuleFilters, ublockUnbreak.htmlRules));
  CHECK(compareNums(parser.numExceptionFilters +
          parser.numNoFingerprintExceptionFilters +
          parser.hostAnchoredExceptionHashSet->size(),
        ublockUnbreak.exceptions));
}

// Should parse brave-unbreak list without failing
TEST(parser, parse_brave_unbreak) {
  string && fileContents = // NOLINT
    getFileContents("./test/data/brave-unbreak.txt");
  ABPFilterParser parser;
  parser.parse(fileContents.c_str());

  CHECK(compareNums(parser.numFilters +
          parser.numNoFingerprintFilters +
          parser.hostAnchoredHashSet->size(),
        braveUnbreak.filters));
  CHECK(compareNums(parser.numHtmlRuleFilters, braveUnbreak.htmlRules));
  CHECK(compareNums(parser.numExceptionFilters +
          parser.numNoFingerprintExceptionFilters +
          parser.hostAnchoredExceptionHashSet->size(),
        braveUnbreak.exceptions));
}

// Should parse disconnect-simple-malware.txt list without failing
TEST(parser, parse_brave_disconnect_simple_malware) {
  string && fileContents = // NOLINT
    getFileContents("./test/data/disconnect-simple-malware.txt");
  ABPFilterParser parser;
  parser.parse(fileContents.c_str());

  CHECK(compareNums(parser.numFilters +
          parser.numNoFingerprintFilters +
          parser.hostAnchoredHashSet->size(),
        disconnectSimpleMalware.filters));
  CHECK(compareNums(parser.numHtmlRuleFilters,
        disconnectSimpleMalware.htmlRules));
  CHECK(compareNums(parser.numExceptionFilters +
          parser.numNoFingerprintExceptionFilters +
          parser.hostAnchoredExceptionHashSet->size(),
        disconnectSimpleMalware.exceptions));
}


// Should parse spam404-main-blacklist.txt list without failing
TEST(parser, parse_spam404_main_blacklist) {
  string && fileContents = // NOLINT
    getFileContents("./test/data/spam404-main-blacklist.txt");
  ABPFilterParser parser;
  parser.parse(fileContents.c_str());

  CHECK(compareNums(parser.numFilters +
          parser.numNoFingerprintFilters +
          parser.hostAnchoredHashSet->size(),
        spam404MainBlacklist.filters));
  CHECK(compareNums(parser.numHtmlRuleFilters, spam404MainBlacklist.htmlRules));
  CHECK(compareNums(parser.numExceptionFilters +
          parser.numNoFingerprintExceptionFilters +
          parser.hostAnchoredExceptionHashSet->size(),
        spam404MainBlacklist.exceptions));

  const char *urlToCheck = "http://excellentmovies.net/";
  const char *currentPageDomain = "excellentmovies.net";
  CHECK(parser.matches(urlToCheck, FODocument, currentPageDomain));
}


// Should parse lists without failing
TEST(parser, parse_multiList) {
  string && fileContentsEasylist = // NOLINT
    getFileContents("./test/data/easylist.txt");

  string && fileContentsUblockUnbreak = // NOLINT
    getFileContents("./test/data/ublock-unbreak.txt");

  string && fileContentsBraveUnbreak = // NOLINT
    getFileContents("./test/data/brave-unbreak.txt");

  ABPFilterParser parser;
  parser.parse(fileContentsEasylist.c_str());
  parser.parse(fileContentsUblockUnbreak.c_str());
  parser.parse(fileContentsBraveUnbreak.c_str());

  // I think counts are slightly off due to same rule hash set

  /*
  CHECK(compareNums(parser.numFilters +
          parser.numNoFingerprintFilters +
          parser.hostAnchoredHashSet->size(),
        easyList.filters +
          ublockUnbreak.filters +
          braveUnbreak.filters));
          */
  CHECK(compareNums(parser.numHtmlRuleFilters,
        easyList.htmlRules +
          ublockUnbreak.htmlRules +
          braveUnbreak.htmlRules));
  /*
  CHECK(compareNums(parser.numExceptionFilters +
          parser.hostAnchoredExceptionHashSet->size() +
          parser.numNoFingerprintExceptionFilters,
        easyList.exceptions +
          ublockUnbreak.exceptions +
          braveUnbreak.exceptions));
  */
}

// Should parse lists without failing
TEST(parser, parse_malware_multiList) {
  string && fileContentsSpam404 = // NOLINT
    getFileContents("./test/data/spam404-main-blacklist.txt");

  string && fileContentsDisconnectSimpleMalware = // NOLINT
    getFileContents("./test/data/disconnect-simple-malware.txt");

  ABPFilterParser parser;
  parser.parse(fileContentsSpam404.c_str());
  parser.parse(fileContentsDisconnectSimpleMalware.c_str());

  // I think counts are slightly off due to same rule hash set

  /*
  CHECK(compareNums(parser.numFilters +
          parser.numNoFingerprintFilters +
          parser.hostAnchoredHashSet->size(),
        disconnectSimpleMalware.filters +
          spam404MainBlacklist.filters));
  */
  CHECK(compareNums(parser.numHtmlRuleFilters,
        disconnectSimpleMalware.htmlRules +
          spam404MainBlacklist.htmlRules));
  CHECK(compareNums(parser.numExceptionFilters +
          parser.hostAnchoredExceptionHashSet->size() +
          parser.numNoFingerprintExceptionFilters,
        disconnectSimpleMalware.exceptions+
          spam404MainBlacklist.exceptions));
}


// Calling parse amongst 2 different lists should preserve both sets of rules
TEST(multipleParse, multipleParse2) {
  ABPFilterParser parser;
  parser.parse("adv\n"
               "@@test\n"
               "###test\n");
  parser.parse("adv2\n"
               "@@test2\n"
               "###test2\n"
               "adv3\n"
               "@@test3\n"
               "###test3");

  CHECK(compareNums(parser.numFilters +
        parser.numNoFingerprintFilters, 3));
  CHECK(compareNums(parser.numHtmlRuleFilters, 3));
  CHECK(compareNums(parser.numExceptionFilters +
        parser.numNoFingerprintExceptionFilters, 3));
}

// Demo app test
TEST(demoApp, demoApp2) {
  ABPFilterParser parser;
  parser.parse("||googlesyndication.com/safeframe/$third-party");
  const char *urlToCheck =
    "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html";
  const char *currentPageDomain = "slashdot.org";
  CHECK(parser.matches(urlToCheck, FOScript, currentPageDomain));
}

TEST(hostAnchoredFiltersParseCorrectly, hostAnchoredFiltersParseCorrectly2) {
  // Host anchor is calculated correctly
  Filter filter;
  parseFilter("||test.com$third-party", &filter);
  CHECK(!strcmp("test.com", filter.host));

  Filter filter2;
  parseFilter("||test.com/ok$third-party", &filter2);
  CHECK(!strcmp("test.com", filter2.host));

  Filter filter3;
  parseFilter("||test.com/ok", &filter3);
  CHECK(!strcmp("test.com", filter3.host));
}

TEST(misc, misc2) {
  for (int i = 0; i < 256; i++) {
    if (i == static_cast<int>(':') || i == static_cast<int>('?') ||
        i == static_cast<int>('/') ||
        i == static_cast<int>('=') || i == static_cast<int>('^') ||
        i == static_cast<int>('$')) {
      CHECK(isSeparatorChar(static_cast<char>(i)));
    } else {
      CHECK(!isSeparatorChar(static_cast<int>(static_cast<char>(i))));
    }
  }
}


TEST(serializationTests, serializationTests2) {
  ABPFilterParser parser;
  parser.parse(
      "||googlesyndication.com$third-party\n@@||googlesyndication.ca");
  int size;
  char * buffer = parser.serialize(&size);

  ABPFilterParser parser2;
  parser2.deserialize(buffer);

  Filter f(static_cast<FilterType>(FTHostAnchored | FTHostOnly), FOThirdParty,
      FONoFilterOption, "googlesyndication.com", 21, nullptr,
      "googlesyndication.com");
  Filter f2(FTNoFilterType, FOThirdParty, FONoFilterOption,
      "googleayndication.com", 21, nullptr, "googleayndication.com");
  CHECK(parser.hostAnchoredHashSet->exists(f));
  CHECK(parser2.hostAnchoredHashSet->exists(f));
  CHECK(!parser.hostAnchoredHashSet->exists(f2));
  CHECK(!parser2.hostAnchoredHashSet->exists(f2));

  Filter f3(static_cast<FilterType>(FTHostAnchored | FTHostOnly | FTException),
      FONoFilterOption, FONoFilterOption, "googlesyndication.ca",
      20, nullptr, "googlesyndication.ca");
  Filter f4(FTNoFilterType, FONoFilterOption, FONoFilterOption,
      "googleayndication.ca", 20, nullptr, "googleayndication.ca");
  CHECK(parser.hostAnchoredExceptionHashSet->exists(f3));
  CHECK(parser2.hostAnchoredExceptionHashSet->exists(f3));
  CHECK(!parser.hostAnchoredExceptionHashSet->exists(f4));
  CHECK(!parser2.hostAnchoredExceptionHashSet->exists(f4));

  delete[] buffer;
}
