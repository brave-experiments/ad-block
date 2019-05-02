/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include <iostream>
#include "./CppUnitLite/TestHarness.h"
#include "./CppUnitLite/Test.h"
#include "./ad_block_client.h"

TEST(preservesTag, basic) {
  const char * filterText =
    "filter$tag=blah\n";
  const char * currentPageDomain = "slashdot.org";
  const char * urlToCheck = "http://www.brianbondy.com/filter?c=a&view=ad&b=2";

  AdBlockClient client;
  client.parse(filterText, true);
  client.addTag("blah");

  Filter *filter;
  Filter *exceptionFilter;

  CHECK(client.findMatchingFilters(urlToCheck, FONoFilterOption,
    currentPageDomain, &filter, &exceptionFilter));
  CHECK(filter);
  CHECK(std::string(filter->tag, filter->tagLen) == "blah")

  int size;
  char* data = client.serialize(&size);

  AdBlockClient client2;
  client2.addTag("blah");
  client2.deserialize(data);
  // For valgrind only
  client2.deserialize(data);

  filter = nullptr;
  exceptionFilter = nullptr;
  CHECK(client2.findMatchingFilters(urlToCheck, FONoFilterOption,
    currentPageDomain, &filter, &exceptionFilter));
  CHECK(filter);
  CHECK(std::string(filter->tag, filter->tagLen) == "blah")
}

TEST(preservesTag, hostAnchored) {
  const char * filterText =
    "||www.brianbondy.com$tag=blah\n";
  const char * currentPageDomain = "slashdot.org";
  const char * urlToCheck = "http://www.brianbondy.com/filter?c=a&view=ad&b=2";

  AdBlockClient client;
  client.parse(filterText, true);
  client.addTag("blah");

  Filter *filter;
  Filter *exceptionFilter;

  CHECK(client.findMatchingFilters(urlToCheck, FONoFilterOption,
    currentPageDomain, &filter, &exceptionFilter));
  CHECK(filter);
  CHECK(std::string(filter->tag, filter->tagLen) == "blah")

  int size;
  char* data = client.serialize(&size);

  AdBlockClient client2;
  client2.addTag("blah");
  client2.deserialize(data);
  // For valgrind only
  client2.deserialize(data);

  filter = nullptr;
  exceptionFilter = nullptr;
  CHECK(client2.findMatchingFilters(urlToCheck, FONoFilterOption,
    currentPageDomain, &filter, &exceptionFilter));
  CHECK(filter);
  CHECK(std::string(filter->tag, filter->tagLen) == "blah")
}

TEST(preservesTag, basicWithDomain) {
  const char * filterText =
    "filter$tag=blah,domain=brianbondy.com|slashdot.org\n";
  const char * currentPageDomain = "slashdot.org";
  const char * urlToCheck = "http://www.brianbondy.com/filter?c=a&view=ad&b=2";

  AdBlockClient client;
  client.parse(filterText, true);
  client.addTag("blah");

  Filter *filter;
  Filter *exceptionFilter;

  CHECK(client.findMatchingFilters(urlToCheck, FONoFilterOption,
    currentPageDomain, &filter, &exceptionFilter));
  CHECK(filter);
  CHECK(std::string(filter->tag, filter->tagLen) == "blah")

  int size;
  char* data = client.serialize(&size);

  AdBlockClient client2;
  client2.addTag("blah");
  client2.deserialize(data);
  // For valgrind only
  client2.deserialize(data);

  filter = nullptr;
  exceptionFilter = nullptr;
  CHECK(client2.findMatchingFilters(urlToCheck, FONoFilterOption,
    currentPageDomain, &filter, &exceptionFilter));
  CHECK(filter);
  CHECK(std::string(filter->tag, filter->tagLen) == "blah")
}

TEST(preservesTag, hostAnchoredWithDomain) {
  const char * filterText =
    "||www.brianbondy.com$domain=brianbondy.com|slashdot.org,tag=blah\n";
  const char * currentPageDomain = "slashdot.org";
  const char * urlToCheck = "http://www.brianbondy.com/filter?c=a&view=ad&b=2";

  AdBlockClient client;
  client.parse(filterText, true);
  client.addTag("blah");

  Filter *filter;
  Filter *exceptionFilter;

  CHECK(client.findMatchingFilters(urlToCheck, FONoFilterOption,
    currentPageDomain, &filter, &exceptionFilter));
  CHECK(filter);
  CHECK(std::string(filter->tag, filter->tagLen) == "blah")

  int size;
  char* data = client.serialize(&size);

  AdBlockClient client2;
  client2.addTag("blah");
  client2.deserialize(data);
  // For valgrind only
  client2.deserialize(data);

  filter = nullptr;
  exceptionFilter = nullptr;
  CHECK(client2.findMatchingFilters(urlToCheck, FONoFilterOption,
    currentPageDomain, &filter, &exceptionFilter));
  CHECK(filter);
  CHECK(std::string(filter->tag, filter->tagLen) == "blah")
}
