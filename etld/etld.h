/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_H_
#define ETLD_H_

#include <string>
#include <sstream>
#include <vector>

namespace Brave {
namespace eTLD {

enum PublicSuffixTextLineType {
  PublicSuffixTextLineTypeNone = 0,
  PublicSuffixTextLineTypeWhitespace,
  PublicSuffixTextLineTypeComment,
  PublicSuffixTextLineTypeException,
  PublicSuffixTextLineTypeStandard
};

typedef std::string Label;

class PublicSuffixTextLineParseResult {
  public:
    PublicSuffixTextLineParseResult(PublicSuffixTextLineType type, PublicSuffixRule rule) {
      type_ = type;
      rule_ = rule;
    }

    PublicSuffixTextLineParseResult(PublicSuffixTextLineType type, const std::string &ruleText, bool isException = false) {
      type_ = type;
      rule_ = PublicSuffixRule(ruleText, isException);
    }

    PublicSuffixTextLineParseResult(PublicSuffixTextLineType type) {
      type_ = type;
    }

    bool isRule() {
      return (type_ == PublicSuffixTextLineTypeException || type_ == PublicSuffixTextLineTypeStandard);
    }

    PublicSuffixTextLineType type() {
      return type_;
    }

    PublicSuffixRule rule() {
      return rule_;
    }

  private:
    PublicSuffixTextLineType type_;
    PublicSuffixRule rule_;
};

PublicSuffixTextLineParseResult parse_public_suffix_text_line(std::string line);

}
}

#endif // ELTD_H_
