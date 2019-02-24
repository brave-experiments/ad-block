/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "etld/internal/public_suffix_rule.h"
#include "etld/internal/public_suffix_rule_set.h"
#include "etld/domain.h"
#include "etld/types.h"
#include "etld/serialization.h"

using brave_etld::internal::PublicSuffixRuleSetMatchResult;

namespace brave_etld {
namespace internal {

PublicSuffixRuleSet::PublicSuffixRuleSet() {}

PublicSuffixRuleSet::PublicSuffixRuleSet(const PublicSuffixRuleSet &rule_set) :
  rules_(rule_set.Rules()) {}

PublicSuffixRuleSet::PublicSuffixRuleSet(
    const std::vector<PublicSuffixRule> &rules) : 
  rules_(rules) {}

bool PublicSuffixRuleSet::Equal(const PublicSuffixRuleSet &rule_set) const {
  if (rules_.size() != rule_set.rules_.size()) {
    return false;
  }

  bool found_match;
  for (auto &local_elm : rules_) {
    found_match = false;
    for (auto &other_elm : rule_set.rules_) {
     if (local_elm.Equals(other_elm)) {
       found_match = true;
       break;
     }
    }
    if (!found_match) {
      return false;
    }
  }

  return true;
}

SerializationResult PublicSuffixRuleSet::Serialize() const {
  std::stringstream serialized_buffer;
  for (auto &elm : rules_) {
    serialized_buffer << elm.Serialize().buffer;
  }

  const std::string buffer_body = serialized_buffer.str();
  const size_t body_len = buffer_body.size();
  const std::string header_str = std::to_string(body_len);
  const size_t header_len = header_str.size();
  const size_t body_start = header_len + 1;
  const size_t buffer_size = body_start + body_len + 1;
  char buffer[buffer_size];

  snprintf(
    buffer,
    buffer_size,
    "%s:%s",
    header_str.c_str(),
    buffer_body.c_str());

  return {
    buffer,
    body_start,
    body_len
  };
}

PublicSuffixRuleSetMatchResult PublicSuffixRuleSet::Match(
    const Domain &domain) const {
  int longest_length = -1;
  int found_length;
  const PublicSuffixRule * longest_rule;
  for (auto &elm : rules_) {
    if (elm.Matches(domain)) {
      found_length = elm.Labels().size();
      if (found_length > longest_length) {
        longest_rule = &elm;
        longest_length = found_length;
      }
    }
  }

  if (longest_length == -1) {
    return {false, nullptr};
  }

  return {true, longest_rule};
}

void PublicSuffixRuleSet::AddRule(const PublicSuffixRule &rule) {
  rules_.push_back(rule);
}

const std::vector<PublicSuffixRule>& PublicSuffixRuleSet::Rules() const {
  return rules_;
}

PublicSuffixRuleSet rule_set_from_serialization(
    const SerializedBuffer &buffer) {
  std::vector<PublicSuffixRule> rules;
  for (auto &elm : deserialize_buffer(buffer)) {
    rules.push_back(rule_from_serialization(elm));
  }
  return PublicSuffixRuleSet(rules);
}

}  // namespace internal
}  // namespace brave_etld
