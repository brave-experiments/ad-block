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
#include "./ad_block_client.h"
#include "./util.h"

#define CHECK_EQ(a, b) CHECK((a) == (b));

using std::cout;

TEST(ruleDefinition, basic) {
  const char * filterText =
    "[Adblock Plus 2.0]\n"
    "&video_ads_\n"
    "&videoadid=\n"
    "&view=ad&\n"
    "+advertorial.\n"
    "+adverts/\n"
    "-2/ads/\n"
    "-2011ad_\n"
    "-300x100ad2.\n"
    "-ad-001-\n"
    "-ad-180x150px.\n"
    "-ad-200x200-\n"
    "! comment here";

  const char * currentPageDomain = "slashdot.org";
  const char * urlToCheck = "http://www.brianbondy.com?c=a&view=ad&b=2";

  AdBlockClient client;
  client.parse(filterText, true);

  Filter none;
  Filter *matchingFilter = &none;
  Filter exceptionFilter;
  Filter *matchingExceptionFilter = &exceptionFilter;

  CHECK(client.findMatchingFilters(urlToCheck, FONoFilterOption,
    currentPageDomain, &matchingFilter, &matchingExceptionFilter));
  CHECK(matchingFilter);
  CHECK_EQ(strcmp(matchingFilter->data, "&view=ad&"), 0);
  CHECK_EQ(matchingExceptionFilter, nullptr);
  CHECK_EQ(strcmp(matchingFilter->ruleDefinition, "&view=ad&"), 0);
}

TEST(ruleDefinitionEmptyByDefault, basic) {
  AdBlockClient client;
  client.parse(
    "-google-analytics.$image,script,xmlhttprequest\n"
    ".net/ad\n",
    false);
  const char * urlToCheck =
    "https://www.scrumpoker.online/js/angular-google-analytics.js";
  const char * currentPageDomain = "slashdot.org";

  Filter none;
  Filter *matchingFilter = &none;
  Filter exceptionFilter;
  Filter *matchingExceptionFilter = &exceptionFilter;

  // Since "false" was passed in as the second argument to `parse`,
  // then no information about the original filters should be retained.
  CHECK(client.findMatchingFilters(urlToCheck, FOScript, currentPageDomain,
    &matchingFilter, &matchingExceptionFilter));
  CHECK(matchingFilter);
  CHECK_EQ(strcmp(matchingFilter->data, "-google-analytics."), 0);
  CHECK_EQ(matchingExceptionFilter, nullptr);
  CHECK_EQ(matchingFilter->ruleDefinition, nullptr);
}

// Test to see if we can parse and restore lines correctly, when there is white
// space at the start of the line.
TEST(ruleDefinitionLeadingWhitespace, basic) {
  const char * filterText = "\n"
    "    [Adblock Plus 2.0]\n"
    "    &video_ads_\n"
    "    &videoadid=\n"
    "    &view=ad&\n"
    "    ! comment here\n"
    "    ";

  const char * currentPageDomain = "slashdot.org";
  const char * urlToCheck = "http://www.brianbondy.com?c=a&view=ad&b=2";

  AdBlockClient client;
  client.parse(filterText, true);

  Filter none;
  Filter *matchingFilter = &none;
  Filter exceptionFilter;
  Filter *matchingExceptionFilter = &exceptionFilter;

  CHECK(client.findMatchingFilters(urlToCheck, FONoFilterOption,
    currentPageDomain, &matchingFilter, &matchingExceptionFilter));
  CHECK(matchingFilter);
  CHECK_EQ(strcmp(matchingFilter->data, "&view=ad&"), 0);
  CHECK_EQ(matchingExceptionFilter, nullptr);
  CHECK_EQ(strcmp(matchingFilter->ruleDefinition, "&view=ad&"), 0);
}


// Testing returning the original filter text
TEST(ruleDefinitionWithOptions, basic) {
  AdBlockClient client;
  client.parse(
    "-google-analytics.$image,script,xmlhttprequest\n"
    ".net/ad\n",
    true);
  const char * urlToCheck =
     "https://www.scrumpoker.online/js/angular-google-analytics.js";
  const char * currentPageDomain = "slashdot.org";

  Filter none;
  Filter *matchingFilter = &none;
  Filter exceptionFilter;
  Filter *matchingExceptionFilter = &exceptionFilter;

  // The matching filter (matchingFilter->data) should match the
  // filter rule itself, while the matchingFilter->ruleDefinition
  // property should include the entire text of the defining
  // rule, including the type and domain options.
  CHECK(client.findMatchingFilters(urlToCheck, FOScript, currentPageDomain,
    &matchingFilter, &matchingExceptionFilter));
  CHECK(matchingFilter);
  CHECK_EQ(strcmp(matchingFilter->data, "-google-analytics."), 0);
  CHECK_EQ(matchingExceptionFilter, nullptr);
  CHECK_EQ(strcmp(
    matchingFilter->ruleDefinition,
    "-google-analytics.$image,script,xmlhttprequest"), 0);
}
