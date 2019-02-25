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
#include "etld/internal/parser.h"
#include "etld/internal/public_suffix_rule.h"
#include "etld/internal/public_suffix_rule_set.h"
#include "etld/serialization.h"

namespace brave_etld {

class Matcher {
 public:
  Matcher() {}
  Matcher(const Matcher &matcher);
  explicit Matcher(std::ifstream* rule_file);
  explicit Matcher(const std::string& rule_text);
  explicit Matcher(const internal::PublicSuffixParseResult& rules);
  Matcher(const internal::PublicSuffixRuleSet& rules,
    const internal::PublicSuffixRuleSet& exception_rules);

  SerializationResult Serialize() const;

  bool Equal(const Matcher& matcher) const;
  DomainInfo Match(const Domain& domain) const;

 private:
  void ConsumeParseResult(const internal::PublicSuffixParseResult& result);
  internal::PublicSuffixRuleSet rules_;
  internal::PublicSuffixRuleSet exception_rules_;
};

Matcher matcher_from_serialization(const SerializedBuffer& buffer);

}  // namespace brave_etld

#endif  // ETLD_MATCHER_H_
