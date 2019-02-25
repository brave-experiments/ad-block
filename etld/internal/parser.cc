/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "etld/internal/parser.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "etld/internal/public_suffix_rule.h"
#include "etld/types.h"

namespace brave_etld {
namespace internal {

PublicSuffixParseResult::PublicSuffixParseResult() {}

PublicSuffixParseResult::PublicSuffixParseResult(
    const PublicSuffixParseResult& results) :
  num_whitespace_lines_(results.num_whitespace_lines_),
  num_comment_lines_(results.num_comment_lines_),
  num_invalid_rules_(results.num_invalid_rules_),
  rules_(results.rules_) {}

const std::vector<PublicSuffixRule>& PublicSuffixParseResult::Rules() const {
  return rules_;
}

void PublicSuffixParseResult::ConsumeParseResult(
    const PublicSuffixTextLineParseResult& result) {
  switch (result.type) {
    case PublicSuffixTextLineTypeRule:
      rules_.push_back(*(result.rule));
      delete result.rule;
      break;

    case PublicSuffixTextLineTypeWhitespace:
      num_whitespace_lines_ += 1;
      break;

    case PublicSuffixTextLineTypeComment:
      num_comment_lines_ += 1;
      break;

    case PublicSuffixTextLineTypeInvalidRule:
      num_invalid_rules_ += 1;
      break;

    case PublicSuffixTextLineTypeNone:
      break;
  }
}

PublicSuffixParseResult parse_rule_file(std::ifstream* rule_file) {
  std::string line;
  PublicSuffixParseResult results;
  while (std::getline(*rule_file, line)) {
    results.ConsumeParseResult(parse_rule_line(line));
  }

  return results;
}

PublicSuffixParseResult parse_rule_text(const std::string& text) {
  std::istringstream stream(text);
  std::string line;
  PublicSuffixParseResult results;
  while (std::getline(stream, line)) {
    results.ConsumeParseResult(parse_rule_line(line));
  }

  return results;
}

PublicSuffixTextLineParseResult parse_rule_line(const std::string& line) {
  // Check to see if this is a comment line.  If so, process no further.
  if (line.find("//") == 0) {
    return PublicSuffixTextLineParseResult(PublicSuffixTextLineTypeComment);
  }

  const size_t first_non_white_space_char = line.find_first_not_of(" ");
  // Next, check to see if the line is only whitespace.
  if (first_non_white_space_char == std::string::npos) {
    return PublicSuffixTextLineParseResult(PublicSuffixTextLineTypeWhitespace);
  }

  try {
    return PublicSuffixTextLineParseResult(
      PublicSuffixTextLineTypeRule,
      new PublicSuffixRule(line));
  } catch (PublicSuffixRuleInputException error) {
    return PublicSuffixTextLineParseResult(PublicSuffixTextLineTypeInvalidRule);
  }
}

}  // namespace internal
}  // namespace brave_etld
