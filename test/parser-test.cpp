#include "CppUnitLite/TestHarness.h"
#include "CppUnitLite/Test.h"
#include "ABPFilterParser.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <algorithm>
#include "util.h"
#include "string.h"
#include <iostream>
#include <set>

using namespace std;

bool testFilter(const char *rawFilter, FilterType expectedFilterType, FilterOption expectedFilterOption, const char *expectedData, set<string> &&blocked, set<string> &&notBlocked) {
  Filter filter;
  parseFilter(rawFilter, filter);

  if (filter.filterOption != expectedFilterOption) {
    cout << "Actual filter option: " << filter.filterOption << endl << "Expected: " << expectedFilterOption << endl;
    return false;
  }

  if (filter.filterType != expectedFilterType) {
    cout << "Actual filter type: " << filter.filterType << endl << "Expected: " << expectedFilterType << endl;
    return false;
  }

  if (strcmp(filter.data, expectedData)) {
    cout << "Actual filter data: " << filter.data << endl << "Expected: " << expectedData << endl;
    return false;
  }

  bool ret = true;
  std::string lastChecked;
  std::for_each(blocked.begin(), blocked.end(), [&filter, &ret, &lastChecked](string const &s) {
    ret = ret && filter.matches(s.c_str());
    lastChecked = s;
  });
  if (!ret) {
    cout << "Should match but did not: " << lastChecked.c_str() << endl;
    return false;
  }

  std::for_each(notBlocked.begin(), notBlocked.end(), [&filter, &ret, &lastChecked](string const &s) {
    ret = ret && !filter.matches(s.c_str());
    lastChecked = s;
  });
  if (!ret) {
    cout << "Should NOT match but did: " << lastChecked.c_str() << endl;
    return false;
  }

  return true;
}

TEST(parser, parseFilterMatchesFilter)
{
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
    }
  ));

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
    }
  ));

  CHECK(testFilter("||ads.example.com^",
    FTHostAnchored,
    FONoFilterOption,
    "ads.example.com^",
    {
      "http://ads.example.com/foo.gif",
      "http://server1.ads.example.com/foo.gif",
      "https://ads.example.com:8000/",
    }, {
      "http://ads.example.com.ua/foo.gif",
      "http://example.com/redirect/http://ads.example.com/",
    }
  ));

  CHECK(testFilter("|http://example.com/|",
    static_cast<FilterType>(FTLeftAnchored | FTRightAnchored),
    FONoFilterOption,
    "http://example.com/",
    {
      "http://example.com/"
    }, {
      "http://example.com/foo.gif",
      "http://example.info/redirect/http://example.com/",
    }
  ));

  CHECK(testFilter("swf|",
    FTRightAnchored,
    FONoFilterOption,
    "swf",
    {
      "http://example.com/annoyingflash.swf",
    },
    {
      "http://example.com/swf/index.html"
    }
  ));

  CHECK(testFilter("|http://baddomain.example/",
    FTLeftAnchored,
    FONoFilterOption,
    "http://baddomain.example/",
    {
     "http://baddomain.example/banner.gif",
    },
    {
      "http://gooddomain.example/analyze?http://baddomain.example",
    }
  ));

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
    }
  ));

  CHECK(testFilter("http://example.com^",
    FTNoFilterType,
    FONoFilterOption,
    "http://example.com^",
    {
      "http://example.com/",
      "http://example.com:8000/ ",
    },
    {}
  ));

  CHECK(testFilter("^example.com^",
    FTNoFilterType,
    FONoFilterOption,
    "^example.com^",
    {
      "http://example.com:8000/foo.bar?a=12&b=%D1%82%D0%B5%D1%81%D1%82",
    },
    {}
  ));
  CHECK(testFilter("^%D1%82%D0%B5%D1%81%D1%82^",
    FTNoFilterType,
    FONoFilterOption,
    "^%D1%82%D0%B5%D1%81%D1%82^",
    {
      "http://example.com:8000/foo.bar?a=12&b=%D1%82%D0%B5%D1%81%D1%82",
    },
    {
      "http://example.com:8000/foo.bar?a=12&b%D1%82%D0%B5%D1%81%D1%823",
    }
  ));
  CHECK(testFilter("^foo.bar^",
    FTNoFilterType,
    FONoFilterOption,
    "^foo.bar^",
    {
      "http://example.com:8000/foo.bar?a=12&b=%D1%82%D0%B5%D1%81%D1%82"
    },
    {
    }
  ));
#ifndef DISABLE_REGEX
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
    }
  ));
#endif
  CHECK(testFilter("||static.tumblr.com/dhqhfum/WgAn39721/cfh_header_banner_v2.jpg",
    FTHostAnchored,
    FONoFilterOption,
    "static.tumblr.com/dhqhfum/WgAn39721/cfh_header_banner_v2.jpg",
    {
      "http://static.tumblr.com/dhqhfum/WgAn39721/cfh_header_banner_v2.jpg"
    },
    {}
  ));

  CHECK(testFilter("||googlesyndication.com/safeframe/$third-party",
    FTHostAnchored,
    FOThirdParty,
    "googlesyndication.com/safeframe/",
    {
      "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html#xpc=sf-gdn-exp-2&p=http%3A//slashdot.org;",
    },
    {}
  ));
  CHECK(testFilter("||googlesyndication.com/safeframe/$third-party,script",
    FTHostAnchored,
    static_cast<FilterOption>(FOThirdParty|FOScript),
    "googlesyndication.com/safeframe/",
    {
      "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html#xpc=sf-gdn-exp-2&p=http%3A//slashdot.org;",
    },
    {}
  ));
}

bool checkMatch(const char *rules, set<string> &&blocked, set<string> &&notBlocked) {
  ABPFilterParser parsers[2];
  char * buffer;
  for (int i = 0; i < 2; i++) {
    ABPFilterParser &parser = parsers[i];
    if (i == 0) {
      parser.parse(rules);
    } else {
      int size;
      buffer = parsers[0].serialize(size);
      parser.deserialize(buffer);
    }

    bool ret = true;
    std::string lastChecked;
    std::for_each(blocked.begin(), blocked.end(), [&parser, &lastChecked, &ret](string const &s) {
      ret = ret && parser.matches(s.c_str());
      lastChecked = s;
    });
    if (!ret) {
      cout << "Should match but did not: " << lastChecked.c_str() << endl;
      return false;
    }

    std::for_each(notBlocked.begin(), notBlocked.end(), [&parser, &ret, &lastChecked](string const &s) {
      ret = ret && !parser.matches(s.c_str());
      lastChecked = s;
    });
    if (!ret) {
      cout << "Should NOT match but did: " << lastChecked.c_str() << endl;
      return false;
    }
    return true;
  }
  delete[] buffer;
}

TEST(parser, exceptionRules)
{
  CHECK(checkMatch("adv\n"
                   "@@advice.",
    {
      "http://example.com/advert.html"
    }, {
      "http://example.com/advice.html",
    }
  ));

  CHECK(checkMatch("@@advice.\n"
                   "adv",
    {
      "http://example.com/advert.html"
    }, {
      "http://example.com/advice.html"
    }
  ));
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
    }
  ));
}

struct OptionRuleData {
  OptionRuleData(const char *testUrl, FilterOption context, const char *contextDomain, bool shouldBlock) {
    this->testUrl = testUrl;
    this->context = context;
    this->contextDomain = contextDomain;
    this->shouldBlock = shouldBlock;
  }

  const char *testUrl;
  FilterOption context;
  const char *contextDomain;
  bool shouldBlock;
};

bool checkOptionRule(const char *rules, set<OptionRuleData*> &&optionTests) {
  ABPFilterParser parser;
  parser.parse(rules);

  bool fail = false;
  std::for_each(optionTests.begin(), optionTests.end(), [&parser, &fail](OptionRuleData * const &data) {
    bool matches = parser.matches(data->testUrl, data->context, data->contextDomain);
    if (matches != data->shouldBlock) {
      cout << "Expected to block: " << data->shouldBlock << endl << "Actual blocks: " << matches << endl;
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
TEST(parser, optionRules)
{
  CHECK(checkOptionRule("||example.com",
    {
      new OptionRuleData("http://example.com", FOThirdParty, nullptr, true),
      new OptionRuleData("http://example2.com", FOThirdParty, nullptr, false),
      new OptionRuleData("http://example.com", FONotThirdParty, nullptr, true),
    }
  ));

  CHECK(checkOptionRule("||example.com^$third-party",
    {
      new OptionRuleData("http://example.com", FOThirdParty, nullptr, true),
      new OptionRuleData("http://example.com", FONotThirdParty, nullptr, false),
      new OptionRuleData("http://ad.example.com", FOThirdParty, nullptr, true),
      new OptionRuleData("http://ad.example.com", FONotThirdParty, nullptr, false),
      new OptionRuleData("http://example2.com", FOThirdParty, nullptr, false),
      new OptionRuleData("http://example2.com", FONotThirdParty, nullptr, false),
      new OptionRuleData("http://example.com.au", FOThirdParty, nullptr, false),
      new OptionRuleData("http://example.com.au", FONotThirdParty, nullptr, false),
    }
  ));

  CHECK(checkOptionRule("||example.com^$third-party,~script",
    {
      new OptionRuleData("http://example.com", static_cast<FilterOption>(FOThirdParty | FOScript), nullptr, false),
      new OptionRuleData("http://example.com", FOOther, nullptr, true),
      new OptionRuleData("http://example2.com", static_cast<FilterOption>(FOThirdParty | FOOther), nullptr, false),
      new OptionRuleData("http://example.com", static_cast<FilterOption>(FONotThirdParty | FOOther), nullptr, false),
    }
  ));

  CHECK(checkOptionRule("adv$domain=example.com|example.net",
    {
      new OptionRuleData("http://example.net/adv", FONoFilterOption, "example.net", true),
      new OptionRuleData("http://somewebsite.com/adv", FONoFilterOption, "example.com", true),
      new OptionRuleData("http://www.example.net/adv", FONoFilterOption, "www.example.net", true),
      new OptionRuleData("http://my.subdomain.example.com/adv", FONoFilterOption, "my.subdomain.example.com", true),
      new OptionRuleData("http://my.subdomain.example.com/adv", FONoFilterOption, "my.subdomain.example.com", true),
      new OptionRuleData("http://example.com/adv", FONoFilterOption, "badexample.com", false),
      new OptionRuleData("http://example.com/adv", FONoFilterOption, "otherdomain.net", false),
      new OptionRuleData("http://example.net/ad", FONoFilterOption, "example.net", false),
    }
  ));

  CHECK(checkOptionRule("adv$domain=example.com|~foo.example.com",
    {
      new OptionRuleData("http://example.net/adv", FONoFilterOption, "example.com", true),
      new OptionRuleData("http://example.net/adv", FONoFilterOption, "foo.example.com", false),
      new OptionRuleData("http://example.net/adv", FONoFilterOption, "www.foo.example.com", false),
    }
  ));

  CHECK(checkOptionRule("adv$domain=~example.com|foo.example.com",
    {
      new OptionRuleData("http://example.net/adv", FONoFilterOption, "example.com", false),
      new OptionRuleData("http://example.net/adv", FONoFilterOption, "foo.example.com", true),
      new OptionRuleData("http://example.net/adv", FONoFilterOption, "www.foo.example.com", true),
    }
  ));

  CHECK(checkOptionRule("adv$domain=~example.com",
    {
      new OptionRuleData("http://example.net/adv", FONoFilterOption, "otherdomain.com", true),
      new OptionRuleData("http://somewebsite.com/adv", FONoFilterOption, "example.com", false),
    }
  ));

  CHECK(checkOptionRule("adv$domain=~example.com|~example.net",
    {
      new OptionRuleData("http://example.net/adv", FONoFilterOption, "example.net", false),
      new OptionRuleData("http://somewebsite.com/adv", FONoFilterOption, "example.com", false),
      new OptionRuleData("http://www.example.net/adv", FONoFilterOption, "www.example.net", false),
      new OptionRuleData("http://my.subdomain.example.com/adv", FONoFilterOption, "my.subdomain.example.com", false),
      new OptionRuleData("http://example.com/adv", FONoFilterOption, "badexample.com", true),
      new OptionRuleData("http://example.com/adv", FONoFilterOption, "otherdomain.net", true),
      new OptionRuleData("http://example.net/ad", FONoFilterOption, "example.net", false),
    }
  ));

  CHECK(checkOptionRule("adv$domain=example.com|~example.net",
    {
      new OptionRuleData("http://example.net/adv", FONoFilterOption, "example.net", false),
      new OptionRuleData("http://somewebsite.com/adv", FONoFilterOption, "example.com", true),
      new OptionRuleData("http://www.example.net/adv", FONoFilterOption, "www.example.net", false),
      new OptionRuleData("http://my.subdomain.example.com/adv", FONoFilterOption, "my.subdomain.example.com", true),
      new OptionRuleData("http://example.com/adv", FONoFilterOption, "badexample.com", false),
      new OptionRuleData("http://example.com/adv", FONoFilterOption, "otherdomain.net", false),
      new OptionRuleData("http://example.net/ad", FONoFilterOption, "example.net", false),
    }
  ));

  CHECK(checkOptionRule("adv$domain=example.com|~foo.example.com,script",
    {
      new OptionRuleData("http://example.net/adv", FOScript, "example.com", true),
      new OptionRuleData("http://example.net/adv", FOScript, "foo.example.com", false),
      new OptionRuleData("http://example.net/adv", FOScript, "www.foo.example.com", false),
      new OptionRuleData("http://example.net/adv", FOOther, "example.com", false),
      new OptionRuleData("http://example.net/adv", FOOther, "foo.example.com", false),
      new OptionRuleData("http://example.net/adv", FOOther, "www.foo.example.com", false),
    }
  ));

  CHECK(checkOptionRule("adv\n"
                        "@@advice.$~script",
    {
      new OptionRuleData("http://example.com/advice.html", FOOther, nullptr, false),
      new OptionRuleData("http://example.com/advice.html", FOScript, nullptr, true),
      new OptionRuleData("http://example.com/advert.html", FOOther, nullptr, true),
      new OptionRuleData("http://example.com/advert.html", FOScript, nullptr, true),
    }
  ));
}

// Should parse EasyList without failing
TEST(parser, parse)
{
  std::string &&fileContents = getFileContents("./test/data/easylist.txt");
  ABPFilterParser parser;
  parser.parse(fileContents.c_str());

  // TODO: Compare to JS lib which says 18096 here for filters + nofingerprint
  CHECK(compareNums(parser.numFilters + parser.numNoFingerprintFilters, 18105));
  // TODO: Compare to JS lib which says 26465 here
  CHECK(compareNums(parser.numHtmlRuleFilters, 26455));
  // TODO: Compare to JS lib which says 2975 here
  CHECK(compareNums(parser.numExceptionFilters, 2975));
}

// Calling parse amongst 2 different lists should preserve both sets of rules
TEST(multipleParse, multipleParse2)
{
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

  CHECK(compareNums(parser.numFilters, 0));
  CHECK(compareNums(parser.numNoFingerprintFilters, 3));
  CHECK(compareNums(parser.numHtmlRuleFilters, 3));
  CHECK(compareNums(parser.numExceptionFilters, 3));
}

// Demo app test
TEST(demoApp, demoApp2)
{
  ABPFilterParser parser;
  parser.parse("||googlesyndication.com/safeframe/$third-party");
  const char *urlToCheck = "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html";
  const char *currentPageDomain = "slashdot.org";
  CHECK(parser.matches(urlToCheck, FOScript, currentPageDomain));
}
