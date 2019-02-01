/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "etld.h"
#include <iostream>
#include <string>

using Brave::eTLD::PublicSuffixTextLineType;
using Brave::eTLD::PublicSuffixTextLineParseResult;


namespace Brave {
namespace eTLD {


// PublicSuffixListMatcher::PublicSuffixListMatcher(const char * suffix_list) {
// }

// This attempts to implement the algorithm described here:
//   https://www.publicsuffix.org/list/
PublicSuffixTextLineParseResult parse_public_suffix_text_line(std::string line) {
  // Check to see if this is a comment line.  If so, process no further.
  if (line.find("//") == 0) {
    return PublicSuffixTextLineParseResult(PublicSuffixTextLineTypeComment);
  }

  size_t first_non_white_space_char = line.find_first_not_of(" ");

  // Next, check to see if the line is only whitespace.
  if (first_non_white_space_char == std::string::npos) {
    return PublicSuffixTextLineParseResult(PublicSuffixTextLineTypeWhitespace);
  }

  if (line.find("!", first_non_white_space_char) == 0) {
    return PublicSuffixTextLineParseResult(
      PublicSuffixTextLineTypeException,
      std::string(line, first_non_white_space_char)
    );
  }

  return PublicSuffixTextLineParseResult(
    PublicSuffixTextLineTypeStandard,
    std::string(line, first_non_white_space_char)
  );
}

} // namespace eTLD
} // namespace Brave


bool testLine(std::string line, PublicSuffixTextLineType type) {
  PublicSuffixTextLineParseResult result = Brave::eTLD::parse_public_suffix_text_line(std::string(line));
  if (result.type() != type) {
    std::cout << "Failed on '" << line << "'" << std::endl;
    return false;
  }
  return true;
}


bool testLineAndRule(std::string line, Brave::eTLD::PublicSuffixTextLineType type, const std::string &expected) {
  PublicSuffixTextLineParseResult result = Brave::eTLD::parse_public_suffix_text_line(std::string(line));
  if (result.type() != type) {
    std::cout << "Failed on '" << line << "'" << std::endl;
    return false;
  }

  return result.rule().toString() == expected;
}




int main() {
  testLine("// this is comment", Brave::eTLD::PublicSuffixTextLineTypeComment);
  testLine("      ", Brave::eTLD::PublicSuffixTextLineTypeWhitespace);
  testLine("!google.com", Brave::eTLD::PublicSuffixTextLineTypeException);
  return 0;
}