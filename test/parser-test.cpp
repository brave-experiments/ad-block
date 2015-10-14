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

bool testFilter( const char *rawFilter, FilterType expectedFilterType, const char *expectedData, set<string> &&blocked, set<string> &&notBlocked) {
  Filter filter;
  parseFilter(rawFilter, filter);

  if (filter.filterType != expectedFilterType) {
    cout << "Actual filter type: " << filter.filterType << endl << "Expected: " << expectedFilterType << endl;
    return false;
  }

  if (strcmp(filter.data, expectedData)) {
    cout << "Actual filter data: " << filter.data << endl << "Expected: " << expectedData << endl;
    return false;
  }
  return true;
}

TEST(x, y)
{
  CHECK(testFilter("/banner/*/img",
    noFilterType,
    "/banner/*/img",
    {
      "http://example.com/banner/foo/img",
      "http://example.com/banner/foo/bar/img?param",
      "http://example.com/banner//img/foo",
      "http://example.com/banner//img.gif",
    }, {
      "http://example.com/banner/",
      "http://example.com/banner/img",
      "http://example.com/img/banner/",
    }
  ));

  CHECK(testFilter("/banner/*/img^",
    noFilterType,
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
    hostAnchored,
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
    static_cast<FilterType>(leftAnchored | rightAnchored),
    "http://example.com/",
    {
      "http://example.com/"
    }, {
      "http://example.com/foo.gif",
      "http://example.info/redirect/http://example.com/",
    }
  ));
  CHECK(testFilter("swf|",
    rightAnchored,
    "swf",
    {
      "http://example.com/annoyingflash.swf",
    },
    {
      "http://example.com/swf/index.html"
    }
  ));
  CHECK(testFilter("|http://baddomain.example/",
    leftAnchored,
    "http://baddomain.example/",
    {
     "http://baddomain.example/banner.gif",
    },
    {
      "http://gooddomain.example/analyze?http://baddomain.example",
    }
  ));
  CHECK(testFilter("||example.com/banner.gif",
    hostAnchored,
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
    noFilterType,
    "http://example.com^",
    {
      "http://example.com/",
      "http://example.com:8000/ ",
    },
    {}
  ));
#if 0
  ["^example.com^", {
    isRegex: false,
    isException: false,
    elementHiding: undefined,
    elementHidingException: undefined,
    hostAnchored: undefined,
    leftAnchored: undefined,
    rightAnchored: undefined,
    options: {},
    data: "^example.com^",
    blocked: [
      "http://example.com:8000/foo.bar?a=12&b=%D1%82%D0%B5%D1%81%D1%82",
    ],
    notBlocked: [],
  }],
  ["^%D1%82%D0%B5%D1%81%D1%82^", {
    isRegex: false,
    isException: false,
    elementHiding: undefined,
    elementHidingException: undefined,
    hostAnchored: undefined,
    leftAnchored: undefined,
    rightAnchored: undefined,
    options: {},
    data: "^%D1%82%D0%B5%D1%81%D1%82^",
    blocked: [
      "http://example.com:8000/foo.bar?a=12&b=%D1%82%D0%B5%D1%81%D1%82",
    ],
    notBlocked: [
      "http://example.com:8000/foo.bar?a=12&b%D1%82%D0%B5%D1%81%D1%823",
    ],
  }],
  ["^foo.bar^", {
    isRegex: false,
    isException: false,
    elementHiding: undefined,
    elementHidingException: undefined,
    hostAnchored: undefined,
    leftAnchored: undefined,
    rightAnchored: undefined,
    options: {},
    data: "^foo.bar^",
    blocked: [
      "http://example.com:8000/foo.bar?a=12&b=%D1%82%D0%B5%D1%81%D1%82"
    ],
    notBlocked: [
    ],
  }],
  ["/banner\\d+/", {
    isRegex: true,
    isException: false,
    elementHiding: undefined,
    elementHidingException: undefined,
    hostAnchored: undefined,
    leftAnchored: undefined,
    rightAnchored: undefined,
    options: {},
    data: "banner\\d+",
    blocked: [
      "banner123",
      "testbanner1",
    ],
    notBlocked: [
      "banners",
      "banners123",
    ],
  }],
  ["||static.tumblr.com/dhqhfum/WgAn39721/cfh_header_banner_v2.jpg", {
    isRegex: false,
    isException: false,
    elementHiding: undefined,
    elementHidingException: undefined,
    hostAnchored: true,
    leftAnchored: undefined,
    rightAnchored: undefined,
    options: {},
    data: "static.tumblr.com/dhqhfum/WgAn39721/cfh_header_banner_v2.jpg",
    blocked: [
      "http://static.tumblr.com/dhqhfum/WgAn39721/cfh_header_banner_v2.jpg",
    ],
    notBlocked: [
    ],
  }],
  ["||googlesyndication.com/safeframe/$third-party", {
    isRegex: false,
    isException: false,
    elementHiding: undefined,
    elementHidingException: undefined,
    hostAnchored: true,
    leftAnchored: undefined,
    rightAnchored: undefined,
    options: { "binaryOptions": ["third-party"] },
    data: "googlesyndication.com/safeframe/",
    blocked: [
      "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html#xpc=sf-gdn-exp-2&p=http%3A//slashdot.org;",
    ],
    notBlocked: [
    ],
  }],
#endif


}
