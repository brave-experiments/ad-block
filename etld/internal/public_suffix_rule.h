/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_INTERNAL_PUBLIC_SUFFIX_RULE_H_
#define ETLD_INTERNAL_PUBLIC_SUFFIX_RULE_H_

#include <string>
#include <vector>
#include "etld/domain.h"
#include "etld/types.h"
#include "etld/serialization.h"

using ::std::string;
using ::std::vector;

namespace brave_etld {
namespace internal {

class PublicSuffixRule {
 public:
  PublicSuffixRule();
  explicit PublicSuffixRule(const string& rule_text);
  PublicSuffixRule(
    const vector<Label>& labels,
    bool is_exception = false,
    bool is_wildcard = false);
  PublicSuffixRule(const PublicSuffixRule& rule) = default;
  ~PublicSuffixRule() = default;

  SerializationResult Serialize() const;

  bool Equals(const PublicSuffixRule& rule) const;
  bool Matches(const Domain& domain) const;
  DomainInfo Apply(const Domain& domain) const;
  string ToString() const;
  string DomainString() const;

  const vector<Label>& Labels() const;
  size_t Length() const;
  const Label& Get(size_t idx) const {
    return labels_[idx];
  }

  bool IsException() const {
    return is_exception_;
  }
  bool IsWildcard() const {
    return is_wildcard_;
  }

 protected:
  vector<Label> labels_;
  bool is_exception_ = false;
  bool is_wildcard_ = false;
};

PublicSuffixRule rule_from_serialization(const SerializedBuffer& buffer);

}  // namespace internal
}  // namespace brave_etld

#endif  // ETLD_INTERNAL_PUBLIC_SUFFIX_RULE_H_
