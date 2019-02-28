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
#include "./etld/internal/parser.h"
#include "./etld/internal/public_suffix_rule.h"
#include "./etld/internal/public_suffix_rule_set.h"
#include "./etld/types.h"
#include "./etld/matcher.h"
#include "./etld/domain.h"

using ::std::string;
using ::std::cout;
using ::std::endl;

using brave_etld::internal::PublicSuffixTextLineType;
using brave_etld::internal::PublicSuffixTextLineParseResult;
using brave_etld::internal::PublicSuffixRuleSetMatchResult;
using brave_etld::internal::PublicSuffixRule;
using brave_etld::internal::PublicSuffixRuleSet;
using brave_etld::internal::PublicSuffixParseResult;
using brave_etld::internal::parse_rule_line;
using brave_etld::internal::parse_rule_text;
using brave_etld::internal::parse_rule_file;
using brave_etld::Domain;
using brave_etld::Matcher;
using brave_etld::DomainInfo;

bool testParsePublicSuffixTestLineNoOps(const string &line,
    PublicSuffixTextLineType expectedType) {
  PublicSuffixTextLineParseResult result = parse_rule_line(line);
  if (result.type != expectedType) {
    cout << "Received unexpected parse result for " << line << endl;
    return false;
  }

  return true;
}

bool testParsePublicSuffixTestLineError(const string &line,
    const string &expected_error) {
  PublicSuffixTextLineParseResult result = parse_rule_line(line);
  if (result.type !=
      PublicSuffixTextLineType::PublicSuffixTextLineTypeInvalidRule) {
    cout << "Expected to receive an error when attempting to parse "
         << "invalid rule '" << line << "' but no error was thrown." << endl;
    return false;
  }

  const size_t error_substr_pos = result.error.find_first_of(expected_error);
  if (error_substr_pos == string::npos) {
    cout << "Caught an unexpected error when parsing " << line << "\n"
          << "Expected '" << expected_error << "'\n"
          << "Received '" << result.error << endl;
    return false;
  }

  return true;
}

bool testParsePublicSuffixTest(
    const string &line,
    std::vector<string> expected_labels,
    bool expected_exception,
    bool expected_wildcard) {
  PublicSuffixRule rule(line);
  PublicSuffixRule currect_rule(expected_labels, expected_exception,
    expected_wildcard);
  return currect_rule.Equals(rule);
}

bool _verifyParsePublicSuffixTest(
    PublicSuffixParseResult result,
    int num_expected_rules,
    int num_expected_exception_rules,
    int num_expected_wildcard_rules,
    int num_expected_whitespace_lines,
    int num_expected_comment_lines,
    int num_expected_invalid_lines) {
  if (num_expected_rules != result.Rules().size()) {
    cout << "Incorrect num of rules parsed, expected '"
         << num_expected_rules << "' "
         << "but found '" << result.Rules().size() << "'" << endl;
    return false;
  }

  int num_found_exception_rules = 0;
  int num_found_wildcard_rules = 0;
  for (auto &elm : result.Rules()) {
    if (elm.IsException()) {
      num_found_exception_rules += 1;
    }
    if (elm.IsWildcard()) {
      num_found_wildcard_rules += 1;
    }
  }

  if (num_found_exception_rules != num_expected_exception_rules) {
    cout << "Found unexpected number of exception rules, expected '"
         << num_expected_exception_rules << "' "
         << "but found '" << num_found_exception_rules << "'" << endl;
    return false;
  }

  if (num_found_exception_rules != num_expected_exception_rules) {
    cout << "Found unexpected number of exception rules, expected '"
         << num_expected_exception_rules << "' "
         << "but found '" << num_found_exception_rules << "'" << endl;
    return false;
  }

  if (result.NumWhitespaceLines() != num_expected_whitespace_lines) {
    cout << "Found unexpected number of whitespace lines, expected '"
         << num_expected_whitespace_lines << "' "
         << "but found '" << result.NumWhitespaceLines() << "'" << endl;
    return false;
  }

  if (result.NumInvalidRules() != num_expected_invalid_lines) {
    cout << "Found unexpected number of invalid rules, expected '"
         << num_expected_invalid_lines << "' "
         << "but found '" << result.NumInvalidRules() << "'" << endl;
    return false;
  }

  if (result.NumCommentLines() != num_expected_comment_lines) {
    cout << "Found unexpected number of comment lines, expected '"
         << num_expected_comment_lines << "' "
         << "but found '" << result.NumCommentLines() << "'" << endl;
    return false;
  }

  return true;
}

bool testParsePublicSuffixTextTest(
    const string &text,
    int num_expected_rules,
    int num_expected_exception_rules,
    int num_expected_wildcard_rules,
    int num_expected_whitespace_lines,
    int num_expected_comment_lines,
    int num_expected_invalid_lines) {
  PublicSuffixParseResult result = parse_rule_text(text);
  return _verifyParsePublicSuffixTest(
    result,
    num_expected_rules,
    num_expected_exception_rules,
    num_expected_wildcard_rules,
    num_expected_whitespace_lines,
    num_expected_comment_lines,
    num_expected_invalid_lines);
}

bool testParsePublicSuffixFileTest(
    const string &file_path,
    int num_expected_rules,
    int num_expected_exception_rules,
    int num_expected_wildcard_rules,
    int num_expected_whitespace_lines,
    int num_expected_comment_lines,
    int num_expected_invalid_lines) {
  std::ifstream rule_file;
  rule_file.open(file_path, std::ifstream::in);
  PublicSuffixParseResult result = parse_rule_file(&rule_file);
  return _verifyParsePublicSuffixTest(
    result,
    num_expected_rules,
    num_expected_exception_rules,
    num_expected_wildcard_rules,
    num_expected_whitespace_lines,
    num_expected_comment_lines,
    num_expected_invalid_lines);
}

bool testMatchingDomainRules(const string &rule_str, const string &domain_str,
    bool should_match) {
  Domain domain(domain_str);
  PublicSuffixRule rule(rule_str);

  bool does_match = rule.Matches(domain);
  if (should_match != does_match) {
    string desc(should_match
      ? "should have, but didn't match "
      : "didn't, but should have matched ");
    cout << "Domain: '" << domain.ToString() << "' "
         << desc
         << "Rule: '" << rule.ToString() << "'" << endl;
    return false;
  }

  return true;
}

bool testPublicSuffixRuleSetMatch(const std::vector<string> &rules,
    const string &domain_str, const string &expected_match) {
  PublicSuffixRuleSet rule_set;
  PublicSuffixRule rule;
  for (auto &elm : rules) {
    rule = PublicSuffixRule(elm);
    rule_set.AddRule(rule);
  }

  Domain test_domain(domain_str);
  PublicSuffixRuleSetMatchResult match_result = rule_set.Match(test_domain);
  if (match_result.found_match == false) {
    cout << "Expected rule: '" << expected_match << "' "
         << "to find a match, but received none." << endl;
    return false;
  }

  string result_string = match_result.rule->DomainString();
  if (result_string != expected_match) {
    cout << "Expected rule: '" << expected_match << "' "
         << "to match '" << domain_str << "' "
         << "but received '" << result_string << "'" << endl;
    return false;
  }

  return true;
}

bool testPublicSuffixRuleSetNoMatch(const std::vector<string> &rules,
    const string &domain_str) {
  PublicSuffixRuleSet rule_set;
  PublicSuffixRule rule;
  for (auto &elm : rules) {
    rule = PublicSuffixRule(elm);
    rule_set.AddRule(rule);
  }

  Domain test_domain(domain_str);
  PublicSuffixRuleSetMatchResult match_result = rule_set.Match(test_domain);
  if (match_result.found_match == true) {
    cout << "Did not expect rule: '" << domain_str << "' to match any "
         << "rules, but found a match." << endl;
    return false;
  }

  return true;
}

bool testPublicSuffixRuleTextApply(const string &rule_str,
    const string &domain_str, const DomainInfo &expected) {
  PublicSuffixRule rule(rule_str);
  Domain domain(domain_str);
  DomainInfo extracted = rule.Apply(domain);
  if (extracted.tld != expected.tld) {
    cout << "Expected tld of '" << expected.tld
         << "' when applying rule '"
         << rule_str << "' to domain '" << domain_str
         << "', but received tld '" << extracted.tld << "'" << endl;
    return false;
  }

  if (extracted.domain != expected.domain) {
    cout << "Expected domain of '" << expected.domain
         << "' when applying rule '"
         << rule_str << "' to domain '" << domain_str
         << "', but received domain '"
         << extracted.domain << "'" << endl;
    return false;
  }

  if (extracted.subdomain != expected.subdomain) {
    cout << "Expected subdomain of '" << expected.subdomain
         << "' when applying rule '"
         << rule_str << "' to domain '" << domain_str
         << "', but received subdomain '"
         << extracted.subdomain << "'" << endl;
    return false;
  }

  return true;
}

bool testPublicSuffixRuleFileApply(const string &file_path,
    const string &domain_str, const DomainInfo &expected) {
  std::ifstream rule_file;
  rule_file.open(file_path, std::ifstream::in);
  Matcher matcher(&rule_file);
  DomainInfo extracted = matcher.Match(Domain(domain_str));

  if (extracted.tld != expected.tld) {
    cout << "Expected tld of '" << expected.tld
         << "' when applying file '"
         << file_path << "' to domain '" << domain_str
         << "', but received tld '" << extracted.tld << "'" << endl;
    return false;
  }

  if (extracted.domain != expected.domain) {
    cout << "Expected domain of '" << expected.domain
         << "' when applying file '"
         << file_path << "' to domain '" << domain_str
         << "', but received domain '" << extracted.domain << "'" << endl;
    return false;
  }

  if (extracted.subdomain != expected.subdomain) {
    cout << "Expected subdomain of '" << expected.subdomain
         << "' when applying file '"
         << file_path << "' to domain '" << domain_str
         << "', but received subdomain '" << extracted.subdomain << "'" << endl;
    return false;
  }

  return true;
}

TEST(eTLDParseRulesLine, basic) {
  CHECK(testParsePublicSuffixTestLineNoOps(
    "// this is an option",
    brave_etld::internal::PublicSuffixTextLineTypeComment));

  CHECK(testParsePublicSuffixTestLineNoOps(
    "this.is.a.invalid..rule",
    brave_etld::internal::PublicSuffixTextLineTypeInvalidRule));

  CHECK(testParsePublicSuffixTestLineNoOps(
    "",
    brave_etld::internal::PublicSuffixTextLineTypeWhitespace));

  // Since we read until the first white space, this will be read
  // as "example.", which is invalid because rules cannot hae empty labels
  CHECK(testParsePublicSuffixTestLineError(
    "example. org",
    "Rules cannot end with a delimiter"));
  CHECK(testParsePublicSuffixTestLineError(
    "example..org",
    "Rules cannot contain adjectent delimitors"));
  CHECK(testParsePublicSuffixTestLineError(
    "example.org.",
    "Rules cannot end with a delimiter"));

  // Examples taken from https://publicsuffix.org/list/
  CHECK(testParsePublicSuffixTest(
    "com",
    {"com"},
    false,
    false));
  CHECK(testParsePublicSuffixTest(
    "*.jp",
    {"*", "jp"},
    false,
    true));
  CHECK(testParsePublicSuffixTest(
    "*.jp",
    {"*", "jp"},
    false,
    true));
  CHECK(testParsePublicSuffixTest(
    "*.tokyo.jp",
    {"*", "tokyo", "jp"},
    false,
    true));
  CHECK(testParsePublicSuffixTest(
    "!pref.hokkaido.jp",
    {"pref", "hokkaido", "jp"},
    true,
    false));
}

TEST(eTLDParseRulesText, basic) {
  // Just retesting the examples from above, to make sure they are parsed
  // correctly when all in a line.
  CHECK(testParsePublicSuffixTextTest(
    "com\n*.jp\n*.jp\n*.tokyo.jp\n!pref.hokkaido.jp",
    5, 1, 3, 0, 0, 0));

  CHECK(testParsePublicSuffixTextTest(
    "// ac : https://en.wikipedia.org/wiki/.ac\n"
    "ac\n"
    "com.ac\n"
    "edu.ac\n"
    "gov.ac\n"
    "net.ac\n"
    "mil.ac\n"
    "org.ac\n",
    7, 0, 0, 0, 1, 0));
  CHECK(testParsePublicSuffixTextTest(
    "// am : https://en.wikipedia.org/wiki/.am\n"
    "am\n"
    "\n"
    "// ao : https://en.wikipedia.org/wiki/.ao \n"
    "// http://www.dns.ao/REGISTR.DOC \n"
    "ao\n"
    "ed.ao\n"
    "gv.ao\n"
    "og.ao\n"
    "co.ao\n"
    "pb.ao\n"
    "it.ao\n",
    8, 0, 0, 1, 3, 0));
  CHECK(testParsePublicSuffixTextTest(
    "\n"
    "*.wild.rule\n"
    "!exception.rule \n"
    "// coment line \n"
    "example.org\n"
    "...badrule",
    3, 1, 1, 1, 1, 1));
}

TEST(eTLDParseRulesFile, basic) {
  CHECK(testParsePublicSuffixFileTest(
    "./test/data/public_suffix_list_short.txt",
    32, 1, 1, 8, 12, 0));
}

TEST(eTLDMatchTests, basic) {
  CHECK(testMatchingDomainRules("com", "com", true));
  CHECK(testMatchingDomainRules("fp.com", "com", false));
  CHECK(testMatchingDomainRules("com", "fp.com", true));
  CHECK(testMatchingDomainRules("*.hokkaido.jp", "bar.hokkaido.jp", true));
  CHECK(testMatchingDomainRules("!pref.hokkaido.jp", "pref.hokkaido.jp", true));
  CHECK(testMatchingDomainRules("*.tokyo.jp", "tokyo.jp", false));
  CHECK(testMatchingDomainRules("*.tokyo.jp", "other.tokyo.jp", true));
  CHECK(testMatchingDomainRules("com", "foo.com", true));
}

TEST(eTLDPublicSuffixRuleSetMatchTests, basic) {
  CHECK(testPublicSuffixRuleSetMatch(
    {"com", "*.jp", "*.hokkaido.jp"},
    "foo.com",
    "com"));
  CHECK(testPublicSuffixRuleSetMatch(
    {"com", "*.jp", "*.hokkaido.jp"},
    "hokkaido.jp",
    "*.jp"));
  CHECK(testPublicSuffixRuleSetMatch(
    {"com", "*.jp", "*.hokkaido.jp"},
    "pete.hokkaido.jp",
    "*.hokkaido.jp"));
  CHECK(testPublicSuffixRuleSetNoMatch(
    {"com", "*.jp", "*.hokkaido.jp"},
    "horse.shoes"));
}

TEST(eTLDPublicSuffixRuleApplyTestTests, basic) {
  DomainInfo test_one_expected_match = {"example.jp", "shoes", "pete"};
  CHECK(testPublicSuffixRuleTextApply(
    "*.jp", "pete.shoes.example.jp", test_one_expected_match));

  DomainInfo test_two_expected_match = {"horse", "the", "we.love"};
  CHECK(testPublicSuffixRuleTextApply(
      "horse", "we.love.the.horse", test_two_expected_match));

  DomainInfo test_three_expected_match = {"tokyo.jp", "metro", "slate"};
  CHECK(testPublicSuffixRuleTextApply(
      "!metro.tokyo.jp", "slate.metro.tokyo.jp", test_three_expected_match));
}

TEST(eTLDPublicSuffixRuleApplyFileTests, basic) {
  DomainInfo file_test_one = {"com", "google", "www"};
  CHECK(testPublicSuffixRuleFileApply(
    "./test/data/public_suffix_list.dat",
    "www.google.com",
    file_test_one));

  DomainInfo file_test_two = {"co.uk", "google", ""};
  CHECK(testPublicSuffixRuleFileApply(
    "./test/data/public_suffix_list.dat",
    "google.co.uk",
    file_test_two));

  // Tests taken from https://www.npmjs.com/package/tldts
  DomainInfo file_test_three = {"s3.amazonaws.com", "spark-public", ""};
  CHECK(testPublicSuffixRuleFileApply(
    "./test/data/public_suffix_list.dat",
    "spark-public.s3.amazonaws.com",
    file_test_three));

  DomainInfo file_test_four = {"org", "writethedocs", "www"};
  CHECK(testPublicSuffixRuleFileApply(
    "./test/data/public_suffix_list.dat",
    "www.writethedocs.org",
    file_test_four));

  // Tests taken from https://www.npmjs.com/package/tld
  DomainInfo file_test_five = {"bar.sch.uk", "foo", "www"};
  CHECK(testPublicSuffixRuleFileApply(
    "./test/data/public_suffix_list.dat",
    "www.foo.bar.sch.uk",
    file_test_five));

  // Tests taken from https://www.npmjs.com/package/parse-domain
  DomainInfo file_test_six = {"co.uk", "example", "some.subdomain"};
  CHECK(testPublicSuffixRuleFileApply(
    "./test/data/public_suffix_list.dat",
    "some.subdomain.example.co.uk",
    file_test_six));

  // Tests adapted from
  // https://raw.githubusercontent.com/publicsuffix/list/master/tests/test_psl.txt
  struct DomainInfoTestCase {
    string input;
    DomainInfo expected;
  };
  DomainInfoTestCase file_test_cases[] = {
    {"com", {"com", "", ""}},
    {"example.com", {"com", "example", ""}},
    {"www.example.com", {"com", "example", "www"}},
    {"example", {"example", "", ""}},
    {"example.example", {"example", "example", ""}},
    {"b.example.example", {"example", "example", "b"}},
    {"a.b.example.example", {"example", "example", "a.b"}},
    {"biz", {"biz", "", ""}},
    {"domain.biz", {"biz", "domain", ""}},
    {"b.domain.biz", {"biz", "domain", "b"}},
    {"uk.com", {"uk.com", "", ""}},
    {"example.uk.com", {"uk.com", "example", ""}},
    {"b.example.uk.com", {"uk.com", "example", "b"}},
    {"a.b.example.uk.com", {"uk.com", "example", "a.b"}},
    {"jp", {"jp", "", ""}},
    {"test.jp", {"jp", "test", ""}},
    {"www.test.jp", {"jp", "test", "www"}},
    {"ac.jp", {"ac.jp", "", ""}},
    {"test.ac.jp", {"ac.jp", "test", ""}},
    {"www.test.ac.jp", {"ac.jp", "test", "www"}},
    {"kyoto.jp", {"kyoto.jp", "", ""}},
    {"test.kyoto.jp", {"kyoto.jp", "test", ""}},
    {"ide.kyoto.jp", {"ide.kyoto.jp", "", ""}},
    {"b.ide.kyoto.jp", {"ide.kyoto.jp", "b", ""}},
    {"a.b.ide.kyoto.jp", {"ide.kyoto.jp", "b", "a"}},
    {"c.kobe.jp", {"c.kobe.jp", "", ""}},
    {"b.c.kobe.jp", {"c.kobe.jp", "b", ""}},
    {"a.b.c.kobe.jp", {"c.kobe.jp", "b", "a"}},
    {"city.kobe.jp", {"kobe.jp", "city", ""}},
    {"www.city.kobe.jp", {"kobe.jp", "city", "www"}},
    {"ck", {"ck", "", ""}},
    {"test.ck", {"test.ck", "", ""}},
    {"b.test.ck", {"test.ck", "b", ""}},
    {"a.b.test.ck", {"test.ck", "b", "a"}},
    {"www.ck", {"ck", "www", ""}},
    {"www.www.ck", {"ck", "www", "www"}},
    {"us", {"us", "", ""}},
    {"test.us", {"us", "test", ""}},
    {"www.test.us", {"us", "test", "www"}},
    {"ak.us", {"ak.us", "", ""}},
    {"test.ak.us", {"ak.us", "test", ""}},
    {"www.test.ak.us", {"ak.us", "test", "www"}},
    {"k12.ak.us", {"k12.ak.us", "", ""}},
    {"test.k12.ak.us", {"k12.ak.us", "test", ""}},
    {"www.test.k12.ak.us", {"k12.ak.us", "test", "www"}},
    {"食狮.com.cn", {"com.cn", "食狮", ""}},
    {"食狮.公司.cn", {"公司.cn", "食狮", ""}},
    {"sushi.公司.cn", {"公司.cn", "sushi", ""}},
    {"公司.cn", {"公司.cn", "", ""}},
    {"食狮.中国", {"中国", "食狮", ""}},
    {"shishi.中国", {"中国", "shishi", ""}},
    {"中国", {"中国", "", ""}},
  };
  for (auto &elm : file_test_cases) {
    CHECK(testPublicSuffixRuleFileApply(
      "./test/data/public_suffix_list.dat",
      elm.input,
      elm.expected));
  }
}
