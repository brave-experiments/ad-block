/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_PUBLIC_SUFFIX_RULE_H_
#define ETLD_PUBLIC_SUFFIX_RULE_H_

#include <string>
#include <vector>
#include "etld/domain.h"
#include "etld/types.h"
#include "etld/serialization.h"

using std::unique_ptr;

namespace brave_etld {

class PublicSuffixRuleInputException : public std::exception {
 public:
  explicit PublicSuffixRuleInputException(const char * message) :
    msg_(message) {}
  explicit PublicSuffixRuleInputException(const std::string &message) :
    msg_(message) {}
  virtual const char* what() const throw() {
    return msg_.c_str();
  }
  virtual ~PublicSuffixRuleInputException() throw() {}

 protected:
  std::string msg_;
};

class PublicSuffixRule {
 public:
  PublicSuffixRule();
  PublicSuffixRule(const PublicSuffixRule &rule);
  explicit PublicSuffixRule(const std::string &rule_text);
  PublicSuffixRule(
    const std::vector<Label> &labels,
    bool is_exception = false,
    bool is_wildcard = false);

  SerializationResult Serialize() const;

  bool Equals(const PublicSuffixRule &rule) const;
  bool Matches(const Domain &domain) const;
  DomainInfo Apply(const Domain &domain) const;
  std::string ToString() const;
  std::string DomainString() const;

  std::vector<Label> Labels() const;
  size_t Length() const;

  bool IsException() const {
    return is_exception_;
  }
  bool IsWildcard() const {
    return is_wildcard_;
  }

 protected:
  std::vector<Label> labels_;
  bool is_exception_ = false;
  bool is_wildcard_ = false;
};

PublicSuffixRule rule_from_serialization(const SerializedBuffer &buffer);
std::vector<Label> parse_labels(const std::string &label_text);

}  // namespace brave_etld

#endif  // ETLD_PUBLIC_SUFFIX_RULE_H_
