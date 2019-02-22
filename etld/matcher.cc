/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <fstream>
#include <sstream>
#include "etld/types.h"
#include "etld/parser.h"
#include "etld/public_suffix_rule.h"
#include "etld/matcher.h"
#include "etld/serialization.h"

namespace brave_etld {

Matcher::Matcher(std::ifstream &rule_file) {
  ConsumeParseResult(parse_rule_file(rule_file));
}

Matcher::Matcher(const std::string &rule_text) {
  ConsumeParseResult(parse_rule_text(rule_text));
}

Matcher::Matcher(const PublicSuffixParseResult &rules) {
  ConsumeParseResult(rules);
}

Matcher::Matcher(const Matcher &matcher) :
  rules_(matcher.rules_),
  exception_rules_(matcher.exception_rules_) {}

Matcher::Matcher(const PublicSuffixRuleSet &rules,
    const PublicSuffixRuleSet &exception_rules) :
  rules_(rules),
  exception_rules_(exception_rules) {}

SerializationResult Matcher::Serialize() const {
  std::stringstream serialized_buffer;
  serialized_buffer << rules_.Serialize().buffer;
  serialized_buffer << exception_rules_.Serialize().buffer;

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

  SerializationResult info;
  info.body_start = body_start;
  info.body_len = body_len;
  info.buffer = buffer;
  return info;
}

bool Matcher::Equal(const Matcher &matcher) const {
  return (rules_.Equal(matcher.rules_) &&
          exception_rules_.Equal(matcher.exception_rules_));
}

// Attempts to implement the algoritms described here:
//   https://publicsuffix.org/list/
DomainInfo Matcher::Match(const Domain &domain) const {
  PublicSuffixRuleSetMatchResult except_match = exception_rules_.Match(domain);
  if (except_match.found_match) {
    return BuildDomainInfo(except_match.rule, domain);
  }

  PublicSuffixRuleSetMatchResult rule_match = rules_.Match(domain);
  if (rule_match.found_match) {
    return BuildDomainInfo(rule_match.rule, domain);
  }

  return BuildDomainInfo(PublicSuffixRule("*"), domain);
}

DomainInfo Matcher::BuildDomainInfo(const PublicSuffixRule &rule,
    const Domain &domain) const {
  return rule.Apply(domain);
}

void Matcher::ConsumeParseResult(const PublicSuffixParseResult &result) {
  for (auto &elm : result.Rules()) {
    if (elm.IsException()) {
      exception_rules_.AddRule(elm);
    } else {
      rules_.AddRule(elm);
    }
  }
}

Matcher matcher_from_serialization(const SerializedBuffer &buffer) {
  SerializedChildBuffers child_buffers = deserialize_buffer(buffer);
  return {
    rule_set_from_serialization(child_buffers[0]),
    rule_set_from_serialization(child_buffers[1])
  };
}

}  // namespace brave_etld
