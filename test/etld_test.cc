/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include <iostream>
#include <set>
#include "./CppUnitLite/TestHarness.h"
#include "./CppUnitLite/Test.h"
#include "./etld.h"

using std::string;
using std::cout;
using std::endl;

using Brave::eTLD::PublicSuffixTextLineType;
using Brave::eTLD::PublicSuffixTextLineParseResult;

bool testParsePublicSuffixTestLineType(const char * line, PublicSuffixTextLineType expectedType) {
  PublicSuffixTextLineParseResult result = Brave::eTLD::parse_public_suffix_text_line(string(line));
  return (result.type() == expectedType);
}

TEST(publicSuffixTest, basic) {
  CHECK(testParsePublicSuffixTestLineType(
      "// this is an option",
      Brave::eTLD::PublicSuffixTextLineTypeStandard
    )
  );
}
