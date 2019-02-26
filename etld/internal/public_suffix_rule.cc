/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "etld/internal/public_suffix_rule.h"
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include "etld/domain.h"
#include "etld/types.h"
#include "etld/serialization.h"

namespace brave_etld {
namespace internal {

const PublicSuffixRule* PublicSuffixRule::root_rule = new PublicSuffixRule("*");

std::vector<Label> parse_labels(const std::string& label_text) {
  std::vector<Label> labels;
  size_t previous = 0;
  size_t current = label_text.find(".");
  Label current_label;
  while (current != std::string::npos) {
    current_label = label_text.substr(previous, current - previous);
    if (current_label.length() == 0) {
      throw PublicSuffixRuleInputException(
        "Rules cannot contain adjectent delimitors: " + label_text);
    }
    labels.push_back(current_label);
    previous = current + 1;
    current = label_text.find(".", previous);
  }

  // If don't include any trailing whitespace, if there is any.
  current_label = label_text.substr(previous, current - previous);
  if (current_label == "") {
    throw PublicSuffixRuleInputException(
      "Rules cannot end with a delimiter: " + label_text);
  }

  labels.push_back(current_label);
  return labels;
}

// Cargo-cult-ed from https://codereview.stackexchange.com/questions/40124/trim-white-space-from-string
std::string trim_to_whitespace(std::string const& str) {
  if (str.empty()) {
    return str;
  }

  const size_t first_scan = str.find_first_of(' ');
  if (first_scan == std::string::npos) {
    return str;
  }

  return str.substr(0, first_scan);
}

PublicSuffixRule::PublicSuffixRule() {}

PublicSuffixRule::PublicSuffixRule(const PublicSuffixRule& rule) :
  labels_(rule.labels_),
  is_exception_(rule.IsException()),
  is_wildcard_(rule.IsWildcard()) {}

PublicSuffixRule::PublicSuffixRule(const std::string& rule_text) {
  std::string trimmed_rule_text(trim_to_whitespace(rule_text));
  if (trimmed_rule_text.length() == 0) {
    throw PublicSuffixRuleInputException(
      "Cannot create PublicSuffixRule from an empty string");
  }

  size_t current = 0;
  switch (trimmed_rule_text[0]) {
    case '*':
      is_wildcard_ = true;
      break;

    case '!':
      is_exception_ = true;
      current += 1;
      break;

    case '/':
      throw PublicSuffixRuleInputException(
        "Rules cannot start with '/': " + rule_text);
      break;

    default:
      break;
  }

  std::string labels_view(trimmed_rule_text, current);
  labels_ = parse_labels(labels_view);
}

PublicSuffixRule::PublicSuffixRule(const std::vector<Label>& labels,
  bool is_exception, bool is_wildcard) :
    labels_(labels),
    is_exception_(is_exception),
    is_wildcard_(is_wildcard) {
}

SerializationResult PublicSuffixRule::Serialize() const {
  const std::string domain_string = DomainString();
  const size_t body_len = domain_string.size() + 2;
  const std::string header_str = std::to_string(body_len);
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

  const std::string serialized_buffer(buffer);
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
  const auto domain_len = domain.Length();
  auto rule_len = Length();
  if (IsException()) {
    rule_len -= 1;
  }

  auto tld_seg_len = rule_len;
  std::stringstream tld_seg;

  auto domain_seg_len = (tld_seg_len == domain_len) ? 0 : 1;
  std::stringstream domain_seg;

  auto sub_domain_seg_len = domain_len - domain_seg_len - tld_seg_len;
  std::stringstream sub_domain_seg;

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

std::string PublicSuffixRule::DomainString() const {
  auto last_label_index = labels_.size() - 1;
  size_t label_index = 0;
  std::stringstream as_string;
  for (auto &elm : labels_) {
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

std::string PublicSuffixRule::ToString() const {
  std::stringstream as_string;
  as_string << "labels: [" << DomainString() << "] ";
  as_string << "is exception: " << (is_exception_ ? "true" : "false") << " ";
  as_string << "is wildcard: " << (is_wildcard_ ? "true" : "false");
  return as_string.str();
}

PublicSuffixRule rule_from_serialization(const SerializedBuffer& buffer) {
  const SerializedBufferInfo info = extract_buffer_info(buffer);
  const std::string body_str = buffer.substr(info.body_start, info.body_len);
  bool is_exception = body_str[0] == 't';
  bool is_wildcard = body_str[1] == 't';
  const std::string label_str(body_str, 2, info.body_len - 2);
  const std::vector<Label> labels = parse_labels(label_str);
  return PublicSuffixRule(labels, is_exception, is_wildcard);
}

}  // namespace internal
}  // namespace brave_etld
