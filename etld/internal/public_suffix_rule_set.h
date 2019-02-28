/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_INTERNAL_PUBLIC_SUFFIX_RULE_SET_H_
#define ETLD_INTERNAL_PUBLIC_SUFFIX_RULE_SET_H_

#include <string>
#include <vector>
#include <map>
#include "etld/domain.h"
#include "etld/types.h"
#include "etld/serialization.h"
#include "etld/internal/public_suffix_rule.h"

using ::std::vector;
using ::std::map;

namespace brave_etld {
namespace internal {

struct PublicSuffixRuleSetMatchResult {
  const bool found_match;
  const PublicSuffixRule* rule;
};

struct PublicSuffixRuleMapNode {
  PublicSuffixRuleMapNode();
  ~PublicSuffixRuleMapNode();
  const PublicSuffixRule* rule = nullptr;
  map<string, PublicSuffixRuleMapNode*>* children = nullptr;
};

class PublicSuffixRuleSet {
 public:
  PublicSuffixRuleSet();
  explicit PublicSuffixRuleSet(const vector<PublicSuffixRule>& rules);
  PublicSuffixRuleSet(const PublicSuffixRuleSet& rule_set);
  ~PublicSuffixRuleSet();

  SerializationResult Serialize() const;
  bool Equal(const PublicSuffixRuleSet& rule_set) const;
  PublicSuffixRuleSetMatchResult Match(const Domain& domain) const;
  void AddRule(const PublicSuffixRule& rule);

 private:
  void AddRule(const PublicSuffixRule& rule, const size_t label_index,
    PublicSuffixRuleMapNode* node);
  void MatchRecursions(const Domain &domain, const size_t label_index,
      const PublicSuffixRule** match, PublicSuffixRuleMapNode* node) const;

  PublicSuffixRuleMapNode* root_ = nullptr;
  vector<PublicSuffixRule*> rules_;
};

PublicSuffixRuleSet rule_set_from_serialization(const SerializedBuffer &buffer);

}  // namespace internal
}  // namespace brave_etld

#endif  // ETLD_INTERNAL_PUBLIC_SUFFIX_RULE_SET_H_
