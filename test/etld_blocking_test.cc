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

using ::std::cout;
using ::std::endl;
using ::std::string;

bool testThirdPartyBlockingWithRulesDomainAndPublixSuffixList(
    const string &rules, const string &url, const string &domain,
    const string &public_suffix_list, const string &expected_rule,
    bool should_match) {
  AdBlockClient client;
  client.parse(rules.c_str(), true);
  client.parsePublicSuffixRules(public_suffix_list.c_str());

  Filter matching_filter;
  Filter *matching_ptr = &matching_filter;
  Filter exception_filter;
  Filter *exception_ptr = &exception_filter;

  bool is_match = client.findMatchingFilters(url.c_str(), FONoFilterOption,
    domain.c_str(), &matching_ptr, &exception_ptr);

  if (is_match != should_match) {
    cout << "Expected " << (should_match ? "a match" : "no match")
         << " but received " << (is_match ? "a match" : "no match")
         << "." << endl;
    return false;
  }

  if (!should_match) {
    return true;
  }

  bool is_expected_rule = strcmp(
    matching_ptr->ruleDefinition, expected_rule.c_str()) == 0;

  if (is_expected_rule) {
    return true;
  }

  cout << "Did not return the expected matching rule.  Expected '"
       << expected_rule << "' but received '"
       << matching_ptr->ruleDefinition << "'" << endl;
  return false;
}

TEST(etldBlockingTests, basic) {
  ::std::ifstream ifs("./test/data/public_suffix_list.dat");
  const string public_suffix_rules((::std::istreambuf_iterator<char>(ifs)),
                                   (::std::istreambuf_iterator<char>()));
  const string third_party_rule = "||s3.amazonaws.com^$third-party";
  const string test_url = "http://www.spark-public.s3.amazonaws.com/ads.txt";

  // This should not result in a match, since the rule only applies to
  // third party domains, and www.spark-public.s3.amazonaws.com
  // and spark-public.s3.amazonaws.com are eTLD+1 equal (s3.amazonaws.com
  // is a eTLD).
  CHECK(testThirdPartyBlockingWithRulesDomainAndPublixSuffixList(
    third_party_rule,
    test_url,
    "spark-public.s3.amazonaws.com",
    public_suffix_rules,
    "-ignored-",
    false));

  // This should match, since spark-public.s3.amazonaws.com
  // and other.spark-public.s3.amazonaws.com are _not_ eTLD+1 (s3.amazonaws.com
  // is a eTLD+1).
  CHECK(testThirdPartyBlockingWithRulesDomainAndPublixSuffixList(
    third_party_rule,
    test_url,
    "other.s3.amazonaws.com",
    public_suffix_rules,
    third_party_rule,
    true));

  // This should match, since the www.spark-public.s3.amazonaws.com
  // and beans.amazonaws.com are not eTLD+1 (amazonaws.com is not a eTLD).
  CHECK(testThirdPartyBlockingWithRulesDomainAndPublixSuffixList(
    third_party_rule,
    test_url,
    "beans.amazonaws.com",
    public_suffix_rules,
    third_party_rule,
    true));

  // This should match, since the www.spark-public.s3.amazonaws.com
  // and example.com are not eTLD+1 (amazonaws.com is not a eTLD).
  CHECK(testThirdPartyBlockingWithRulesDomainAndPublixSuffixList(
    third_party_rule,
    test_url,
    "example.com",
    public_suffix_rules,
    third_party_rule,
    true));

  // Last, add some non-matching rules, just to make sure we can still
  // pick out correct rules.
  CHECK(testThirdPartyBlockingWithRulesDomainAndPublixSuffixList(
    third_party_rule + "\n||not-matching-anything.com\ntrees^",
    test_url,
    "example.com",
    public_suffix_rules,
    third_party_rule,
    true));
}

bool testAdBlockSerializationWithPublicSuffixList(const string &rules,
    const string &url, const string &domain, const string &public_suffix_list,
    const string &expected_rule, bool should_match) {
  AdBlockClient client;
  client.parse(rules.c_str(), true);
  client.parsePublicSuffixRules(public_suffix_list.c_str());

  Filter matching_filter;
  Filter *matching_ptr = &matching_filter;
  Filter exception_filter;
  Filter *exception_ptr = &exception_filter;

  bool one_match = client.findMatchingFilters(url.c_str(), FONoFilterOption,
    domain.c_str(), &matching_ptr, &exception_ptr);

  // First make sure the first client behaves as expected.
  if (one_match != should_match) {
    cout << "Expected " << (should_match ? "a match" : "no match")
         << " with the first client, but received "
         << (one_match ? "a match" : "no match") << "." << endl;
    return false;
  }

  if (should_match) {
    bool is_expected_rule = strcmp(
      matching_ptr->ruleDefinition, expected_rule.c_str()) == 0;
    if (!is_expected_rule) {
      cout << "Client one did not return the expected matching rule. "
           << " Expected '" << expected_rule << "' but received '"
           << matching_ptr->ruleDefinition << "'" << endl;
      return false;
    }
  }

  // And now build a second client from the de-serialized first, and make sure
  // they behave the same way.
  Filter matching_filter2;
  Filter *matching_ptr2 = &matching_filter2;
  Filter exception_filter2;
  Filter *exception_ptr2 = &exception_filter2;

  int size;
  char* buffer = client.serialize(&size);

  AdBlockClient client2;
  client2.deserialize(buffer);

  bool two_match = client2.findMatchingFilters(url.c_str(), FONoFilterOption,
    domain.c_str(), &matching_ptr2, &exception_ptr2);

  if (one_match != two_match) {
    cout << "Client one matched the filter correctly, but client two "
         << "returned " << (two_match ? "a match" : "no match")
         << ". " << endl;
    return false;
  }

  if (two_match) {
    bool returned_rules_match = strncmp(matching_ptr->data,
      matching_ptr2->data, matching_ptr->dataLen) == 0;
    if (!returned_rules_match) {
      cout << "Client two returned differnet rule data ("
           << string(matching_ptr2->data, matching_ptr2->dataLen)
           << ") from client one ("
           << string(matching_ptr->data, matching_ptr->dataLen)
           << ")." << endl;
      return false;
    }
  }

  return true;
}

TEST(etldSerializationTests, basic) {
  ::std::ifstream ifs("./test/data/public_suffix_list.dat");
  const string public_suffix_rules((::std::istreambuf_iterator<char>(ifs)),
                                   (::std::istreambuf_iterator<char>()));
  const string third_party_rule = "||s3.amazonaws.com^$third-party";
  const string test_url = "http://www.spark-public.s3.amazonaws.com/ads.txt";

  // This should not result in a match, since the rule only applies to
  // third party domains, and www.spark-public.s3.amazonaws.com
  // and spark-public.s3.amazonaws.com are eTLD+1 equal (s3.amazonaws.com
  // is a eTLD).
  CHECK(testAdBlockSerializationWithPublicSuffixList(
    third_party_rule,
    test_url,
    "spark-public.s3.amazonaws.com",
    public_suffix_rules,
    "-ignored-",
    false));

  // This should match, since spark-public.s3.amazonaws.com
  // and other.spark-public.s3.amazonaws.com are _not_ eTLD+1 (s3.amazonaws.com
  // is a eTLD+1).
  CHECK(testAdBlockSerializationWithPublicSuffixList(
    third_party_rule,
    test_url,
    "other.s3.amazonaws.com",
    public_suffix_rules,
    third_party_rule,
    true));

  // This should match, since the www.spark-public.s3.amazonaws.com
  // and beans.amazonaws.com are not eTLD+1 (amazonaws.com is not a eTLD).
  CHECK(testAdBlockSerializationWithPublicSuffixList(
    third_party_rule,
    test_url,
    "beans.amazonaws.com",
    public_suffix_rules,
    third_party_rule,
    true));

  // This should match, since the www.spark-public.s3.amazonaws.com
  // and example.com are not eTLD+1 (amazonaws.com is not a eTLD).
  CHECK(testAdBlockSerializationWithPublicSuffixList(
    third_party_rule,
    test_url,
    "example.com",
    public_suffix_rules,
    third_party_rule,
    true));

  // Last, add some non-matching rules, just to make sure we can still
  // pick out correct rules.
  CHECK(testAdBlockSerializationWithPublicSuffixList(
    third_party_rule + "\n||not-matching-anything.com\ntrees^",
    test_url,
    "example.com",
    public_suffix_rules,
    third_party_rule,
    true));
}
