/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "etld/matcher.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include "etld/types.h"
#include "etld/internal/parser.h"
#include "etld/internal/public_suffix_rule.h"
#include "etld/serialization.h"

using ::std::ifstream;
using ::std::string;
using ::std::stringstream;

using brave_etld::internal::PublicSuffixRule;
using brave_etld::internal::PublicSuffixParseResult;
using brave_etld::internal::PublicSuffixRuleSetMatchResult;
using brave_etld::internal::SerializedChildBuffers;

namespace brave_etld {

namespace {
const PublicSuffixRule* ROOT_RULE = new PublicSuffixRule("*");
}  // namespace

DomainInfo build_domain_info(const internal::PublicSuffixRule* rule,
  const Domain& domain) {
  return rule->Apply(domain);
}

Matcher::Matcher() {}

Matcher::Matcher(ifstream* rule_file) {
  ConsumeParseResult(internal::parse_rule_file(rule_file));
}

Matcher::Matcher(const string& rule_text) {
  ConsumeParseResult(internal::parse_rule_text(rule_text));
}

Matcher::Matcher(const PublicSuffixParseResult& rules) {
  ConsumeParseResult(rules);
}

Matcher::Matcher(const internal::PublicSuffixRuleSet& rules,
    const internal::PublicSuffixRuleSet& exception_rules) :
  rules_(rules),
  exception_rules_(exception_rules) {}

SerializationResult Matcher::Serialize() const {
  stringstream buffer_stream;
  buffer_stream << rules_.Serialize().buffer;
  buffer_stream << exception_rules_.Serialize().buffer;

  const string buffer_body = buffer_stream.str();
  const size_t body_len = buffer_body.size();
  const string header_str = ::std::to_string(body_len);
  const size_t header_len = header_str.size();
  const size_t body_start = header_len + 1;
  const size_t buffer_size = body_start + body_len + 1;
  char* buffer = reinterpret_cast<char*>(malloc(sizeof(char) * buffer_size));

  snprintf(
    buffer,
    buffer_size,
    "%s:%s",
    header_str.c_str(),
    buffer_body.c_str());

  const std::string serialized_buffer(buffer);
  free(buffer);

  return {
    serialized_buffer,
    body_start,
    body_len
  };
}

bool Matcher::Equal(const Matcher& matcher) const {
  return (rules_.Equal(matcher.rules_) &&
    exception_rules_.Equal(matcher.exception_rules_));
}

// Attempts to implement the algoritms described here:
//   https://publicsuffix.org/list/
DomainInfo Matcher::Match(const Domain& domain) const {
  PublicSuffixRuleSetMatchResult except_match = exception_rules_.Match(domain);
  if (except_match.found_match) {
    return build_domain_info(except_match.rule, domain);
  }

  PublicSuffixRuleSetMatchResult rule_match = rules_.Match(domain);
  if (rule_match.found_match) {
    return build_domain_info(rule_match.rule, domain);
  }

  return build_domain_info(ROOT_RULE, domain);
}

void Matcher::ConsumeParseResult(const PublicSuffixParseResult& result) {
  for (const auto &elm : result.Rules()) {
    if (elm.IsException()) {
      exception_rules_.AddRule(elm);
    } else {
      rules_.AddRule(elm);
    }
  }
}

Matcher matcher_from_serialization(const SerializedBuffer& buffer) {
  SerializedChildBuffers child_buffers = internal::deserialize_buffer(buffer);
  return {
    internal::rule_set_from_serialization(child_buffers[0]),
    internal::rule_set_from_serialization(child_buffers[1])
  };
}

}  // namespace brave_etld
