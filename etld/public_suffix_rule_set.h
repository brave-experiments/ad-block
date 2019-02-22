/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_PUBLIC_SUFFIX_RULE_SET_H_
#define ETLD_PUBLIC_SUFFIX_RULE_SET_H_

#include <string>
#include <vector>
#include "etld/public_suffix_rule.h"
#include "etld/domain.h"
#include "etld/types.h"
#include "etld/serialization.h"

namespace brave_etld {

struct PublicSuffixRuleSetMatchResult {
  bool found_match;
  PublicSuffixRule rule;
};

class PublicSuffixRuleSet {
 public:
  PublicSuffixRuleSet();
  PublicSuffixRuleSet(const std::vector<PublicSuffixRule> &rules);
  PublicSuffixRuleSet(const PublicSuffixRuleSet &rule_set);

  SerializationResult Serialize() const;
  bool Equal(const PublicSuffixRuleSet &rule_set) const;
  PublicSuffixRuleSetMatchResult Match(const Domain &domain) const;
  void AddRule(const PublicSuffixRule &rule);
  std::vector<PublicSuffixRule> Rules() const;

 private:
  std::vector<PublicSuffixRule> rules_;
};

PublicSuffixRuleSet rule_set_from_serialization(const SerializedBuffer &buffer);

}  // namespace brave_etld

#endif  // ETLD_PUBLIC_SUFFIX_RULE_SET_H_
