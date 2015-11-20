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

bool testCosmeticFilter(const char *rawFilter, FilterType expectedFilterType,const char *expectedData) {
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

TEST(parser, parseCosmeticFilters)
{
  CHECK(testCosmeticFilter("###A9AdsMiddleBoxTop", FTElementHiding, "#A9AdsMiddleBoxTop"));
  CHECK(testCosmeticFilter("#@#A9AdsMiddleBoxTop", FTElementHidingException, "#A9AdsMiddleBoxTop"));
}