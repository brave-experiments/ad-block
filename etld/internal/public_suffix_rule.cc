/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "etld/internal/public_suffix_rule.h"
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include <iostream>
#include "etld/internal/parser.h"
#include "etld/domain.h"
#include "etld/types.h"
#include "etld/serialization.h"

using ::std::string;
using ::std::stringstream;
using ::std::vector;

namespace brave_etld {
namespace internal {

PublicSuffixRule::PublicSuffixRule() {}

PublicSuffixRule::PublicSuffixRule(const string& rule_text) {
  PublicSuffixTextLineParseResult parse_result = parse_rule_line(rule_text);
  // This will only ever be hit in tests, since external code should never
  // initialize rules directly, and this initializer receiving
  // bad rules is guarded by code in the parser.
  if (parse_result.type != PublicSuffixTextLineTypeRule) {
    PublicSuffixRule();
    return;
  }

  is_wildcard_ = parse_result.rule->is_wildcard_;
  is_exception_ = parse_result.rule->is_exception_;
  labels_ = parse_result.rule->labels_;
}

PublicSuffixRule::PublicSuffixRule(const vector<Label>& labels,
  bool is_exception, bool is_wildcard) :
    labels_(labels),
    is_exception_(is_exception),
    is_wildcard_(is_wildcard) {
}

SerializationResult PublicSuffixRule::Serialize() const {
  const string domain_string = DomainString();
  const size_t body_len = domain_string.size() + 2;
  const string header_str = ::std::to_string(body_len);
  const size_t header_len = header_str.size();
  const size_t body_start = header_len + 1;
  const uint buffer_size = body_start + body_len + 1;
  char* buffer = reinterpret_cast<char*>(malloc(sizeof(char) * buffer_size));

  snprintf(
    buffer,
    buffer_size,
    "%s:%c%c%s",
    header_str.c_str(),
    is_exception_ ? 't' : 'f',
    is_wildcard_ ? 't' : 'f',
    domain_string.c_str());

  const string serialized_buffer(buffer);
  free(buffer);

  return {
    serialized_buffer,
    body_start,
    body_len
  };
}

bool PublicSuffixRule::Equals(const PublicSuffixRule& rule) const {
  return (
    labels_ == rule.labels_ &&
    is_exception_ == rule.IsException() &&
    is_wildcard_ == rule.IsWildcard());
}

// Implements matching algoritms described here
//   https://publicsuffix.org/list/
bool PublicSuffixRule::Matches(const Domain& domain) const {
  // When the domain and rule are split into corresponding labels, that the
  // domain contains as many or more labels than the rule.
  const size_t num_rule_labels = labels_.size();
  const size_t num_domain_labels = domain.Length();
  if (num_rule_labels > num_domain_labels) {
    return false;
  }

  // Beginning with the right-most labels of both the domain and the rule, and
  // continuing for all labels in the rule, one finds that for every pair,
  // either they are identical, or that the label from the rule is "*".
  for (size_t i = 0; i < num_rule_labels; i += 1) {
    const Label& rule_label = labels_[num_rule_labels - i - 1];
    const Label& domain_label = domain.Get(num_domain_labels - i - 1);

    if (rule_label == "*") {
      continue;
    }

    if (rule_label != domain_label) {
      return false;
    }
  }

  return true;
}

DomainInfo PublicSuffixRule::Apply(const Domain& domain) const {
  const size_t domain_len = domain.Length();
  size_t rule_len = Length();
  if (IsException()) {
    rule_len -= 1;
  }

  auto tld_seg_len = rule_len;
  stringstream tld_seg;

  auto domain_seg_len = (tld_seg_len == domain_len) ? 0 : 1;
  stringstream domain_seg;

  auto sub_domain_seg_len = domain_len - domain_seg_len - tld_seg_len;
  stringstream sub_domain_seg;

  Label domain_label;
  for (size_t i = 0; i < domain_len; i += 1) {
    domain_label = domain.Get(i);

    if (sub_domain_seg_len > 0) {
      sub_domain_seg_len -= 1;
      sub_domain_seg << domain_label;
      if (sub_domain_seg_len != 0) {
        sub_domain_seg << ".";
      }
      continue;
    }

    if (domain_seg_len > 0) {
      domain_seg_len -= 1;
      domain_seg << domain_label;
      if (domain_seg_len != 0) {
        domain_seg << ".";
      }
      continue;
    }

    tld_seg_len -= 1;
    tld_seg << domain_label;
    if (tld_seg_len != 0) {
      tld_seg << ".";
    }
  }

  return {
    tld_seg.str(),
    domain_seg.str(),
    sub_domain_seg.str()
  };
}

string PublicSuffixRule::DomainString() const {
  const size_t last_label_index = labels_.size() - 1;
  size_t label_index = 0;
  stringstream as_string;
  for (const auto &elm : labels_) {
    as_string << elm;
    if (label_index != last_label_index) {
      as_string << ".";
    }
    label_index += 1;
  }

  return as_string.str();
}

const std::vector<Label>& PublicSuffixRule::Labels() const {
  return labels_;
}

size_t PublicSuffixRule::Length() const {
  return labels_.size();
}

string PublicSuffixRule::ToString() const {
  stringstream as_string;
  as_string << "labels: [" << DomainString() << "] ";
  as_string << "is exception: " << (is_exception_ ? "true" : "false") << " ";
  as_string << "is wildcard: " << (is_wildcard_ ? "true" : "false");
  return as_string.str();
}

PublicSuffixRule rule_from_serialization(const SerializedBuffer& buffer) {
  const SerializedBufferInfo info = extract_buffer_info(buffer);
  const string body_str = buffer.substr(info.body_start, info.body_len);
  const bool is_exception = body_str[0] == 't';
  const bool is_wildcard = body_str[1] == 't';
  const string label_str(body_str, 2, info.body_len - 2);
  const PublicSuffixTextLabelsParseResult label_rs = parse_labels(label_str);
  return {label_rs.labels, is_exception, is_wildcard};
}

}  // namespace internal
}  // namespace brave_etld
