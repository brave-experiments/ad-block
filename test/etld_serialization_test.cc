/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include "./CppUnitLite/TestHarness.h"
#include "./CppUnitLite/Test.h"
#include "./ad_block_client.h"
#include "./etld/types.h"
#include "./etld/matcher.h"
#include "./etld/parser.h"
#include "./etld/domain.h"
#include "./etld/public_suffix_rule.h"
#include "./etld/public_suffix_rule_set.h"
#include "./etld/serialization.h"

using std::string;
using std::vector;
using brave_etld::PublicSuffixRule;
using brave_etld::PublicSuffixRuleSet;
using brave_etld::Matcher;
using brave_etld::SerializationResult;
using brave_etld::rule_from_serialization;
using brave_etld::rule_set_from_serialization;
using brave_etld::matcher_from_serialization;
using brave_etld::SerializedBuffer;

bool testSerializeRule(const string &rule_text) {
  PublicSuffixRule rule(rule_text);
  SerializationResult res = rule.Serialize();
  PublicSuffixRule second_rule = rule_from_serialization(res.buffer);
  return rule.Equals(second_rule);
}

TEST(eTLDSerializationSingleRule, basic) {
  CHECK(testSerializeRule("example.com"));
  CHECK(testSerializeRule("www.example.com"));
  CHECK(testSerializeRule("*.example.com"));
  CHECK(testSerializeRule("!tree.example.com"));
}

bool testSerializeRuleSet(const vector<PublicSuffixRule> &rules) {
  PublicSuffixRuleSet rule_set(rules);
  SerializationResult res = rule_set.Serialize();
  PublicSuffixRuleSet second_set = rule_set_from_serialization(res.buffer);
  return rule_set.Equal(second_set);
}

TEST(eTLDSerializationRuleSet, basic) {
  CHECK(testSerializeRuleSet({PublicSuffixRule("!one.com")}));
  CHECK(testSerializeRuleSet({
    PublicSuffixRule("!one.com"), PublicSuffixRule("*.two.com")}));
  CHECK(testSerializeRuleSet({
    PublicSuffixRule("!one.com"), PublicSuffixRule("*.two.com"),
    PublicSuffixRule("three.com")}));
}

bool testSerializeMatcher(const string &rule_text) {
  Matcher orig(rule_text);
  SerializationResult res = orig.Serialize();
  Matcher second = matcher_from_serialization(res.buffer);
  return orig.Equal(second);
}

TEST(eTLDSerializationMatcher, basic) {
  CHECK(testSerializeMatcher("!one.com"));
  CHECK(testSerializeMatcher("!one.com\n*.two.com"));
  CHECK(testSerializeMatcher("!one.com\n*.two.com\nthree.com"));
}

bool testMatcherSerializationTransitivity(const string &public_suffix_rules) {
  Matcher orig(public_suffix_rules);
  SerializationResult res = orig.Serialize();

  Matcher second = matcher_from_serialization(res.buffer);
  SerializationResult res2 = second.Serialize();

  if (res2.buffer.compare(res.buffer) != 0) {
    std::cout << "The serialized buffer of the initial Matcher does not match "
              << "the serialized of the second, buffer initialized buffer."
              << std::endl;
    return false;
  }

  return true;
}

bool testAdBlockClientSerializationTransitivity(const string &filter_rules,
    const string &public_suffix_rules) {
  AdBlockClient client;
  client.parse(filter_rules.c_str());
  client.parsePublicSuffixRules(public_suffix_rules.c_str());

  int size;
  char* buffer = client.serialize(&size);

  AdBlockClient client2;
  client2.deserialize(buffer);
  int size2;
  char* buffer2 = client2.serialize(&size2);

  if (size != size2) {
    std::cout << "The size of the serialized buffers of the first and second "
              << "client do not match." << std::endl;
    return false;
  }

  if (strncmp(buffer, buffer2, size) != 0) {
    std::cout << "The serialized buffers of the first and second client do "
              << "not have the same contents." << std::endl;
    return false;
  }

  return true;
}

TEST(eTLDSerializationTransitivity, basic) {
  std::ifstream ifs("./test/data/public_suffix_list_short.txt");
  const string public_suffix_rules((std::istreambuf_iterator<char>(ifs)),
                                   (std::istreambuf_iterator<char>()));
  const string filer_rules = "||not-matching-anything.com\ntrees^";
  CHECK(testMatcherSerializationTransitivity(public_suffix_rules));
  CHECK(testAdBlockClientSerializationTransitivity(
    filer_rules, public_suffix_rules));
}
