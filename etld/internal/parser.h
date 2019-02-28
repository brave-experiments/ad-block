/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_INTERNAL_PARSER_H_
#define ETLD_INTERNAL_PARSER_H_

#include <string>
#include <fstream>
#include <vector>
#include "etld/internal/public_suffix_rule.h"
#include "etld/types.h"

using ::std::ifstream;
using ::std::string;
using ::std::vector;

namespace brave_etld {
namespace internal {

enum PublicSuffixTextLineType {
  PublicSuffixTextLineTypeNone = 0,
  PublicSuffixTextLineTypeWhitespace,
  PublicSuffixTextLineTypeComment,
  PublicSuffixTextLineTypeInvalidRule,
  PublicSuffixTextLineTypeRule,
};

struct PublicSuffixTextLabelsParseResult {
  PublicSuffixTextLabelsParseResult(const string error = "");
  PublicSuffixTextLabelsParseResult(const vector<Label>& labels);
  const bool is_success = false;
  const string error;
  const vector<Label> labels;
};

struct PublicSuffixTextLineParseResult {
  PublicSuffixTextLineParseResult(
    PublicSuffixTextLineType type = PublicSuffixTextLineTypeNone,
    const PublicSuffixRule* rule = nullptr,
    const string error = "");
  const PublicSuffixTextLineType type;
  const PublicSuffixRule* rule;
  const string error;
};

class PublicSuffixParseResult {
 public:
  PublicSuffixParseResult();
  PublicSuffixParseResult(const PublicSuffixParseResult& results) = default;
  ~PublicSuffixParseResult() = default;

  const vector<PublicSuffixRule>& Rules() const;
  void ConsumeParseResult(const PublicSuffixTextLineParseResult& result);

  int NumWhitespaceLines() const {
    return num_whitespace_lines_;
  }

  int NumCommentLines() const {
    return num_comment_lines_;
  }

  int NumInvalidRules() const {
    return num_invalid_rules_;
  }

 private:
  int num_whitespace_lines_ = 0;
  int num_comment_lines_ = 0;
  int num_invalid_rules_ = 0;
  vector<PublicSuffixRule> rules_;
};

// This attempts to implement the algorithm described here:
//   https://www.publicsuffix.org/list/
PublicSuffixParseResult parse_rule_file(ifstream* rule_file);
PublicSuffixParseResult parse_rule_text(const string& rule_text);
PublicSuffixTextLineParseResult parse_rule_line(const string& line);
string trim_to_whitespace(const string &rule_text);
PublicSuffixTextLabelsParseResult parse_labels(const string& label_text);

}  // namespace internal
}  // namespace brave_etld

#endif  // ETLD_INTERNAL_PARSER_H_
