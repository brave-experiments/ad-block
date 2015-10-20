#include "CppUnitLite/TestHarness.h"
#include "CppUnitLite/test.h"
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
      //"http://ads.example.com/foo.gif",
      //"http://server1.ads.example.com/foo.gif",
      //"https://ads.example.com:8000/",
    }, {
      //"http://ads.example.com.ua/foo.gif",
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
#if 0
  CHECK(testFilter("/banner\\d+/",
    regex,
    FONoFilterOption,
    "banner\\d+",
    {
      "banner123",
      "testbanner1"
    },
    {
      "banners",
      "banners123",
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
  ABPFilterParser parser;
  parser.parse(rules);

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
    ret = ret && parser.matches(s.c_str());
    lastChecked = s;
  });
  if (!ret) {
    cout << "Should NOT match but did: " << lastChecked.c_str() << endl;
    return false;
  }
  return true;
}

TEST(parser, exceptionRules)
{
/*
  let exceptionRules = new Map([
  [`adv
    @@advice.`, {
    blocked: [
      'http://example.com/advert.html',
    ],
    notBlocked: [
      'http://example.com/advice.html',
    ]
  }],
  [`@@advice.
    adv`, {
      blocked: [
        'http://example.com/advert.html'
      ],
      notBlocked: [
        'http://example.com/advice.html'
      ],
  }],
  [`@@|http://example.com
    @@advice.
    adv
    !foo`, {
     blocked: [
       'http://examples.com/advert.html',
     ],
     notBlocked: [
       'http://example.com/advice.html',
       'http://example.com/advert.html',
       'http://examples.com/advice.html',
       'http://examples.com/#!foo',
     ],
  }],
]);
*/
}



// Should parse EasyList without failing
TEST(parser, parse)
{
  std::string &&fileContents = getFileContents("./test/data/easylist.txt");
  ABPFilterParser parser;
  parser.parse(fileContents.c_str());

  // TODO: Compare to JS lib which says 18096 here for filters + nofingerprint
  CHECK(compareNums(parser.numFilters, 21022));
  // TODO: Compare to JS lib which says 26465 here
  CHECK(compareNums(parser.numHtmlRuleFilters, 26455));
  // TODO: Compare to JS lib which says 2975 here
  CHECK(compareNums(parser.numExceptionFilters, 58));
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
  // TODO: Should actually be: noFingerprintRules 3, numFilters: 0
  CHECK(compareNums(parser.numFilters, 3));
  CHECK(compareNums(parser.numHtmlRuleFilters, 3));
  CHECK(compareNums(parser.numExceptionFilters, 3));
}
