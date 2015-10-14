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

bool testFilter( const char *rawFilter, const char *expectedData, set<string> &&blocked, set<string> &&notBlocked) {
  Filter filter;
  parseFilter(rawFilter, filter);
  return !strcmp(filter.data, expectedData);
}

TEST(x, y)
{
  CHECK(testFilter("/banner/*/img",
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
}
