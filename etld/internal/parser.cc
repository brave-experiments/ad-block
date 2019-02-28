/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "etld/internal/parser.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include "etld/internal/public_suffix_rule.h"
#include "etld/types.h"

using ::std::ifstream;
using ::std::istringstream;
using ::std::string;
using ::std::vector;

namespace brave_etld {
namespace internal {

PublicSuffixTextLabelsParseResult::PublicSuffixTextLabelsParseResult(
    const string error) : 
      is_success(false),
      error(error) {}

PublicSuffixTextLabelsParseResult::PublicSuffixTextLabelsParseResult(
  const vector<Label>& labels) :
      is_success(true),
      error(""),
      labels(labels) {}

PublicSuffixTextLineParseResult::PublicSuffixTextLineParseResult(
  PublicSuffixTextLineType type, const PublicSuffixRule* rule,
  const string error) :
    type(type),
    rule(rule),
    error(error) {}

PublicSuffixParseResult::PublicSuffixParseResult() {}

const vector<PublicSuffixRule>& PublicSuffixParseResult::Rules() const {
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

PublicSuffixParseResult parse_rule_file(ifstream* rule_file) {
  string line;
  PublicSuffixParseResult results;
  while (::std::getline(*rule_file, line)) {
    results.ConsumeParseResult(parse_rule_line(line));
  }

  return results;
}

PublicSuffixParseResult parse_rule_text(const string& text) {
  istringstream stream(text);
  string line;
  PublicSuffixParseResult results;
  while (::std::getline(stream, line)) {
    results.ConsumeParseResult(parse_rule_line(line));
  }

  return results;
}

PublicSuffixTextLineParseResult parse_rule_line(const string& line) {
  // Check to see if this is a comment line.  If so, process no further.
  if (line.find("//") == 0) {
    return {PublicSuffixTextLineTypeComment};
  }

  const size_t first_non_white_space_char = line.find_first_not_of(" ");
  // Next, check to see if the line is only whitespace.
  if (first_non_white_space_char == string::npos) {
    return {PublicSuffixTextLineTypeWhitespace};
  }

  string trimmed_rule_text(trim_to_whitespace(line));
  if (trimmed_rule_text.length() == 0) {
    return {
      PublicSuffixTextLineTypeInvalidRule,
      nullptr, 
      "Cannot create PublicSuffixRule from an empty string"
    };
  }

  bool is_wildcard = false;
  bool is_exception = false;

  size_t current = 0;
  switch (trimmed_rule_text[0]) {
    case '*':
      is_wildcard = true;
      break;

    case '!':
      is_exception = true;
      current += 1;
      break;

    case '/':
      return {
        PublicSuffixTextLineTypeInvalidRule,
        nullptr, 
        "Rules cannot start with '/'"
      };
      break;

    default:
      break;
  }

  string labels_view(trimmed_rule_text, current);
  const auto label_result(parse_labels(labels_view));
  if (!label_result.is_success) {
    return {
      PublicSuffixTextLineTypeInvalidRule,
      nullptr,
      label_result.error
    };
  }

  return {
    PublicSuffixTextLineTypeRule,
    new PublicSuffixRule(label_result.labels, is_exception, is_wildcard)
  };
}

string trim_to_whitespace(string const& str) {
  if (str.empty()) {
    return str;
  }

  const size_t first_scan = str.find_first_of(' ');
  if (first_scan == string::npos) {
    return str;
  }

  return str.substr(0, first_scan);
}

PublicSuffixTextLabelsParseResult parse_labels(const string& label_text) {
  vector<Label> labels;
  size_t previous = 0;
  size_t current = label_text.find(".");
  Label current_label;
  while (current != string::npos) {
    current_label = label_text.substr(previous, current - previous);
    if (current_label.length() == 0) {
      return {
        "Rules cannot contain adjectent delimitors: " + label_text
      };
    }
    labels.push_back(current_label);
    previous = current + 1;
    current = label_text.find(".", previous);
  }

  // If don't include any trailing whitespace, if there is any.
  current_label = label_text.substr(previous, current - previous);
  if (current_label == "") {
    return {
      "Rules cannot end with a delimiter: " + label_text
    };
  }

  labels.push_back(current_label);
  return {labels};
}

}  // namespace internal
}  // namespace brave_etld
