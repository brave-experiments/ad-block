#include "CppUnitLite/TestHarness.h"
#include "ABPFilterParser.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include "util.h"
#include "string.h"
#include <iostream>
#include <set>

using namespace std;

void printSet(set<string> &domainSet) {
  std::for_each(domainSet.begin(), domainSet.end(), [](string const &s) {
    cout << s.c_str() << " ";
  });
}

bool testOptions(const char *rawOptions, FilterOption expectedOption, FilterOption expectedAntiOption, set<string> &&expectedDomains, set<string> &&expectedAntiDomains) {
  Filter f;
  f.parseOptions(rawOptions);
  if (f.filterOption != expectedOption) {
    cout << rawOptions << endl << "Actual options: " << f.filterOption  << endl << "Expected: " << expectedOption << endl;
    return false;
  }
  if (f.antiFilterOption != expectedAntiOption) {
    cout << rawOptions << endl << "Actual anti options: " << f.antiFilterOption << endl << "Expected: " << expectedAntiOption << endl;
    return false;
  }
  if (expectedDomains.size() != f.getDomainCount()) {
    cout << rawOptions << endl << "Actual domain count: " << f.getDomainCount() << endl << "Expected: " << expectedDomains.size() << endl;
    return false;
  }
  if (expectedAntiDomains.size() != f.getDomainCount(true)) {
    cout << rawOptions << endl << "Actual anti domain count: " << f.getDomainCount(false) << endl << "Expected: " << expectedAntiDomains.size() << endl;
    return false;
  }

  bool ret = true;
  std::for_each(expectedDomains.begin(), expectedDomains.end(), [&f, &expectedDomains, &ret, rawOptions](string const &s) {
    if (!f.containsDomain(s.c_str())) {
      cout << rawOptions << endl << "Actual domains: " << (f.domainList ? f.domainList : "") << endl << "Expected: ";
      printSet(expectedDomains);
      cout << endl;
      cout << "Not found: " << s.c_str() << endl;
      ret = false;
    }
  });
  if (!ret) {
    return false;
  }

  std::for_each(expectedAntiDomains.begin(), expectedAntiDomains.end(), [&f, &expectedAntiDomains, &ret, rawOptions](string const &s) {
    if (!f.containsDomain(s.c_str(), true)) {
      cout << rawOptions << endl << "Actual anti domains: " << (f.domainList ? f.domainList : "") << endl << "Expected: ";
      printSet(expectedAntiDomains);
      cout << endl;
      ret = false;
    }
  });
  if (!ret) {
    return false;
  }

  return true;
}

TEST(options, test)
{
  CHECK(testOptions("subdocument,third-party",
    static_cast<FilterOption>(FOThirdParty | FOSubdocument),
    FONoFilterOption,
    {},
    {}
  ));

  CHECK(testOptions("object-subrequest,script,domain=~msnbc.msn.com|~www.nbcnews.com",
    static_cast<FilterOption>(FOObjectSubrequest | FOScript),
    FONoFilterOption,
    {},
    {
      "msnbc.msn.com",
      "www.nbcnews.com"
    }
  ));

  CHECK(testOptions("~document,xbl,domain=~foo|bar|baz|foo.xbl|gar,~collapse",
    FOXBL,
    static_cast<FilterOption>(FODocument | FOCollapse),
    {
      "bar",
      "baz",
      "foo.xbl",
      "gar"
    },
    {
      "foo"
    }
  ));
}

/*
 *
// Maps option strings to [set of binary options, domains, skipDomains]
let splitOptions = new Map([
,, , , ["domain=~example.com|foo.example.com,script", [
    new Set(["script"]),
    ["foo.example.com"],
    ["example.com"]
  ]],
]);

// Maps option strings to [domains, skipDomains]
let domainOptionStrings = new Map([
  ["domain=example.com", [
    ["example.com"],
    []
  ]], ["domain=example.com|example.net", [
    ["example.com", "example.net"],
    []
  ]], ["domain=~example.com", [
    [],
    ["example.com"],
  ]], ["domain=example.com|~foo.example.com", [
    ["example.com"],
    ["foo.example.com"]
  ]], ["domain=~foo.example.com|example.com", [
    ["example.com"],
    ["foo.example.com"],
  ]],
  ["domain=example.com|example.net", [
    ["example.com", "example.net"],
    [],
  ]],
  ["domain=example.com|~foo.example.com", [
    ["example.com"],
    ["foo.example.com"],
  ]],
  ["domain=~msnbc.msn.com|~www.nbcnews.com", [
    [],
    ["msnbc.msn.com", "www.nbcnews.com"],
  ]],
]);

let parseOptionTests = new Map([
  ["domain=foo.bar", [
    undefined,
    undefined,
    undefined,
  ]], ["+Ads/$~stylesheet", [
    new Set(["~stylesheet"]),
    undefined,
    undefined,
  ]], ["-advertising-$domain=~advertise.bingads.domain.com", [
    new Set(),
    [],
    ["advertise.bingads.domain.com"],
  ]], [".se/?placement=$script,third-party", [
    new Set(["script", "third-party"]),
    undefined,
    undefined,
  ]], ["||tst.net^$object-subrequest,third-party,domain=domain1.com|domain5.com", [
    new Set(["object-subrequest", "third-party"]),
    ["domain1.com", "domain5.com"],
    [],
  ]],
]);

describe("options#parseOptions()", function() {
  it("Option parsing should split options properly", function() {
    splitOptions.forEach(([expectedOptions, domains, skipDomains], optionsString) => {
      let options = parseOptions(optionsString);
      assert.equal(JSON.stringify(options.binaryOptions), JSON.stringify(expectedOptions));
      assert.equal(JSON.stringify(options.domains), JSON.stringify(domains));
      assert.equal(JSON.stringify(options.skipDomains), JSON.stringify(skipDomains));
    });
  });
  it("domain rule types should be properly parsed", function() {
    domainOptionStrings.forEach(([domains, skipDomains], optionsString) => {
      let options = parseOptions(optionsString);
      assert.equal(JSON.stringify(options.domains), JSON.stringify(domains));
      assert.equal(JSON.stringify(options.skipDomains), JSON.stringify(skipDomains));
    });
  });
  it("parseFilter for full rules properly extracts options", function() {
    parseOptionTests.forEach(([expectedOptions, domains, skipDomains], filterString) => {
      let parsedFilterOptions = {};
      parseFilter(filterString, parsedFilterOptions);
      assert.equal(JSON.stringify(parsedFilterOptions.options.binaryOptions), JSON.stringify(expectedOptions));
      assert.equal(JSON.stringify(parsedFilterOptions.options.domains), JSON.stringify(domains));
      assert.equal(JSON.stringify(parsedFilterOptions.options.skipDomains), JSON.stringify(skipDomains));
    });
  });
});
*/
