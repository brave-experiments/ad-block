/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_MATCHER_H_
#define ETLD_MATCHER_H_

#include <string>
#include <fstream>
#include <vector>
#include "etld/domain.h"
#include "etld/types.h"
#include "etld/parser.h"
#include "etld/public_suffix_rule.h"
#include "etld/public_suffix_rule_set.h"
#include "etld/serialization.h"

namespace brave_etld {

class Matcher {
 public:
  Matcher() {}
  Matcher(const Matcher &matcher);
  explicit Matcher(std::ifstream &rule_file);
  explicit Matcher(const std::string &rule_text);
  explicit Matcher(const PublicSuffixParseResult &rules);
  Matcher(const PublicSuffixRuleSet &rules, const PublicSuffixRuleSet &exception_rules);

  SerializationResult Serialize() const;

  bool Equal(const Matcher &matcher) const;
  void ConsumeParseResult(const PublicSuffixParseResult &result);
  DomainInfo Match(const Domain &domain) const;

  size_t NumRules() const {
    return rules_.Rules().size();
  }

  size_t NumExceptionRules() const {
    return exception_rules_.Rules().size();
  }

 private:
  DomainInfo BuildDomainInfo(const PublicSuffixRule &rule,
    const Domain &domain) const;
  PublicSuffixRuleSet rules_;
  PublicSuffixRuleSet exception_rules_;
};

Matcher matcher_from_serialization(const SerializedBuffer &buffer);

}  // namespace brave_etld

#endif  // ETLD_MATCHER_H_
