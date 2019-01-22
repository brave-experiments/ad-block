/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <set>
#include <string>
#include <iostream>
using namespace std;

#ifdef ENABLE_REGEX
#include <regex> // NOLINT
#endif

#include "./filter.h"
#include "hashFn.h"
#include "./ad_block_client.h"

#include "BloomFilter.h"

static HashFn h(19);

const char * getUrlHost(const char *input, int *len);

Filter::Filter() :
  borrowed_data(false),
  filterType(FTNoFilterType),
  filterOption(FONoFilterOption),
  antiFilterOption(FONoFilterOption),
  ruleDefinition(nullptr),
  data(nullptr),
  dataLen(-1),
  domainList(nullptr),
  host(nullptr),
  hostLen(-1),
  domainsParsed(false) {
}

Filter::~Filter() {
  if (borrowed_data) {
    return;
  }
  if (data) {
    delete[] data;
  }
  if (ruleDefinition) {
    delete[] ruleDefinition;
  }
  if (domainList) {
    delete[] domainList;
  }
  if (host) {
    delete[] host;
  }
}

Filter::Filter(const char * data, int dataLen, char *domainList,
               const char * host, int hostLen) :
      borrowed_data(true), filterType(FTNoFilterType),
      filterOption(FONoFilterOption),
      antiFilterOption(FONoFilterOption), ruleDefinition(nullptr),
      data(const_cast<char*>(data)), dataLen(dataLen),
      domainList(domainList), host(const_cast<char*>(host)),
      hostLen(hostLen),
      domainsParsed(false) {
  }

Filter::Filter(FilterType filterType, FilterOption filterOption,
               FilterOption antiFilterOption,
               const char * data, int dataLen,
               char *domainList, const char * host,
               int hostLen) :
      borrowed_data(true), filterType(filterType),
      filterOption(filterOption),
      antiFilterOption(antiFilterOption), ruleDefinition(nullptr),
      data(const_cast<char*>(data)), dataLen(dataLen),
      domainList(domainList), host(const_cast<char *>(host)),
      hostLen(hostLen),
      domainsParsed(false) {
  }

Filter::Filter(const Filter &other) {
  borrowed_data = other.borrowed_data;
  filterType = other.filterType;
  filterOption = other.filterOption;
  antiFilterOption = other.antiFilterOption;
  dataLen = other.dataLen;
  hostLen = other.hostLen;
  domainsParsed = other.domainsParsed;
  domains = other.domains;
  antiDomains = other.antiDomains;
  if (other.dataLen == -1 && other.data) {
    dataLen = static_cast<int>(strlen(other.data));
  }

  if (other.borrowed_data) {
    data = other.data;
    domainList = other.domainList;
    host = other.host;
    ruleDefinition = other.ruleDefinition;
  } else {
    if (other.data) {
      data = new char[dataLen];
      memcpy(data, other.data, dataLen);
    } else {
      data = nullptr;
    }
    if (other.domainList) {
       size_t len = strlen(other.domainList) + 1;
       domainList = new char[len];
       snprintf(domainList, len, "%s", other.domainList);
    } else {
      domainList = nullptr;
    }
    if (other.host) {
      size_t len = strlen(other.host) + 1;
      host = new char[len];
      snprintf(host, len, "%s", other.host);
    } else {
      host = nullptr;
    }

    if (other.ruleDefinition) {
      size_t len = strlen(other.ruleDefinition) + 1;
      ruleDefinition = new char[len];
      snprintf(ruleDefinition, len, "%s", other.ruleDefinition);
    } else {
      ruleDefinition = nullptr;
    }
  }
}

void Filter::swapData(Filter *other) {
  FilterType tempFilterType = filterType;
  FilterOption tempFilterOption = filterOption;
  FilterOption tempAntiFilterOption = antiFilterOption;
  char *tempData = data;
  int tempDataLen = dataLen;
  char *tempRuleDefinition = ruleDefinition;
  char *tempDomainList = domainList;
  char *tempHost = host;
  int tempHostLen = hostLen;

  filterType = other->filterType;
  filterOption = other->filterOption;
  antiFilterOption = other->antiFilterOption;
  ruleDefinition = other->ruleDefinition;;
  data = other->data;
  dataLen = other->dataLen;
  domainList = other->domainList;
  host = other->host;
  hostLen = other->hostLen;

  other->filterType = tempFilterType;
  other->filterOption = tempFilterOption;
  other->antiFilterOption = tempAntiFilterOption;
  other->ruleDefinition = tempRuleDefinition;
  other->data = tempData;
  other->dataLen = tempDataLen;
  other->domainList = tempDomainList;
  other->host = tempHost;
  other->hostLen = tempHostLen;
}

bool isDomain(const char *input, int len, const char *domain, bool anti) {
  const char *p = input;
  if (anti) {
    if (len >= 1 && p[0] != '~') {
      return false;
    } else {
      len--;
      p++;
    }
  }
  return !memcmp(p, domain, len);
}

bool Filter::containsDomain(const std::string& domain, bool anti) const {
  if (!anti) {
    return std::find(domains.begin(), domains.end(), std::string(domain)) != domains.end();
  }
  return std::find(antiDomains.begin(), antiDomains.end(), std::string(domain)) != antiDomains.end();
}

uint32_t Filter::getDomainCount(bool anti) {
  parseDomains(domainList);
  if (anti) {
    return antiDomains.size();
  }
  return domains.size();
}

bool Filter::isDomainOnlyFilter() {
  parseDomains(domainList);
  return domains.size() && !antiDomains.size();
}

bool Filter::isAntiDomainOnlyFilter() {
  parseDomains(domainList);
  return antiDomains.size() && !domains.size();
}

void Filter::parseOption(const char *input, int len) {
  FilterOption *pFilterOption = &filterOption;
  const char *pStart = input;
  if (input[0] == '~') {
    pFilterOption = &antiFilterOption;
    pStart++;
    len--;
  }

  if (len >= 7 && !strncmp(pStart, "domain=", 7)) {
    len -= 7;
    domainList = new char[len + 1];
    domainList[len] = '\0';
    memcpy(domainList, pStart + 7, len);
  } else if (!strncmp(pStart, "script", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOScript);
  } else if (!strncmp(pStart, "image", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOImage);
  } else if (!strncmp(pStart, "stylesheet", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOStylesheet);
  } else if (!strncmp(pStart, "object", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOObject);
  } else if (!strncmp(pStart, "xmlhttprequest", len)) {
    *pFilterOption =
      static_cast<FilterOption>(*pFilterOption | FOXmlHttpRequest);
  } else if (!strncmp(pStart, "object-subrequest", len)) {
    *pFilterOption =
      static_cast<FilterOption>(*pFilterOption | FOObjectSubrequest);
  } else if (!strncmp(pStart, "subdocument", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOSubdocument);
  } else if (!strncmp(pStart, "document", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FODocument);
  } else if (!strncmp(pStart, "xbl", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOXBL);
  } else if (!strncmp(pStart, "collapse", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOCollapse);
  } else if (!strncmp(pStart, "donottrack", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FODoNotTrack);
  } else if (!strncmp(pStart, "other", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOOther);
  } else if (!strncmp(pStart, "elemhide", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOElemHide);
  } else if (!strncmp(pStart, "third-party", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOThirdParty);
  } else if (!strncmp(pStart, "first-party", len)) {
    // Same as ~third-party
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FONotThirdParty);
  } else if (!strncmp(pStart, "ping", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOPing);
  } else if (!strncmp(pStart, "popup", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOPopup);
  } else if (len >= 4 && !strncmp(pStart, "csp=", 4)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FORedirect);
  } else if (len >= 9 && !strncmp(pStart, "redirect=", 9)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FORedirect);
  } else if (!strncmp(pStart, "font", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOFont);
  } else if (!strncmp(pStart, "media", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOMedia);
  } else if (!strncmp(pStart, "webrtc", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOWebRTC);
  } else if (!strncmp(pStart, "generichide", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOGenericHide);
  } else if (!strncmp(pStart, "genericblock", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOGenericBlock);
  } else if (!strncmp(pStart, "empty", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOEmpty);
  } else if (!strncmp(pStart, "websocket", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOWebsocket);
  } else if (!strncmp(pStart, "important", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOImportant);
  } else {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOUnknown);
    std::string option(pStart, len);
    if (unknownOptions.find(option) == unknownOptions.end()) {
      std::cout << "Unrecognized filter option: " << option << std::endl;
      unknownOptions.insert(option);
    }
  }
  // Otherwise just ignore the option, maybe something new we don't support yet
}

void Filter::parseOptions(const char *input) {
  filterOption = FONoFilterOption;
  antiFilterOption = FONoFilterOption;
  int startOffset = 0;
  int len = 0;
  const char *p = input;
  while (*p != '\0' && !isEndOfLine(*p)) {
    if (*p == ',') {
      parseOption(input + startOffset, len);
      startOffset += len + 1;
      len = -1;
    }
    p++;
    len++;
  }
  parseOption(input + startOffset, len);
}

bool endsWith(const char *input, const char *sub, int inputLen, int subLen) {
  if (subLen > inputLen) {
    return false;
  }

  int startCheckPos = inputLen - subLen;
  const char *p = input + startCheckPos;
  const char *q = sub;
  while (q != sub + subLen) {
    if (*(p++) != *(q++)) {
      return false;
    }
  }
  return true;
}

bool isThirdPartyHost(const char *baseContextHost, int baseContextHostLen,
    const char *testHost, int testHostLen) {
  if (!endsWith(testHost, baseContextHost, testHostLen, baseContextHostLen)) {
    return true;
  }

  // baseContextHost matches testHost exactly
  if (testHostLen == baseContextHostLen) {
    return false;
  }

  char c = testHost[testHostLen - baseContextHostLen - 1];
  return c != '.' && testHostLen != baseContextHostLen;
}

bool Filter::hasUnsupportedOptions() const {
  return (filterOption & FOUnsupportedSoSkipCheck) != 0;
}

bool Filter::contextDomainMatchesFilter(const char *contextDomain) {
  parseDomains(domainList);
  // If there are no context domains, then this filter can still apply
  // to all domains.
  if (domains.size() == 0 && antiDomains.size() == 0) {
    return true;
  }

  const char *p = contextDomain;
  // Start keeps track of the start of the last match
  // We do this to avoid extra TLD checks for rules.
  const char *start = contextDomain;
  while (*p != '\0') {
    if (*p == '.') {
      std::string domain(start);
      if (containsDomain(domain, false)) {
        return true;
      }
      if (containsDomain(domain, true)) {
        return false;
      }
      // Set start to just past the period and increment p
      start = ++p;
    }
    p++;
  }

  // No exact match, if there are only anti domain filters, then this
  // rule applies.
  if (domains.size() == 0 && antiDomains.size() > 0) {
    return true;
  }

  // Otherwise there are only domains, and we haven't matched anything
  // so it's not a match.
  return domains.size() < antiDomains.size();
}

// Determines if there's a match based on the options, this doesn't
// mean that the filter rule should be accepted, just that the filter rule
// should be considered given the current context.
// By specifying context params, you can filter out the number of rules
// which are considered.
bool Filter::matchesOptions(const char *input, FilterOption context,
    const char *contextDomain) {
  if (hasUnsupportedOptions()) {
    return false;
  }

  // If the context is for a document, but the filter option isn't an explicit
  // document rule, then ignore it.
  if (!(filterOption & FODocument) && (context & FODocument)) {
    return false;
  }
  // Maybe the user of the library can't determine a context because they're
  // blocking a the HTTP level, don't block here because we don't have enough
  // information
  if (context != FONoFilterOption) {
    if ((filterOption & ~FOThirdParty) != FONoFilterOption
        && !(filterOption & FOResourcesOnly & context)) {
      return false;
    }

    if ((antiFilterOption & ~FOThirdParty) != FONoFilterOption
        && (antiFilterOption & FOResourcesOnly & context)) {
      return false;
    }
  } else {
    // When there's no filter option specified for the context, the resource
    // type context is not known.  In this case, never match against
    // rules with an explicit resource type specified.
    if (filterOption & FOResourcesOnly) {
      return false;
    }
    if (antiFilterOption & FOResourcesOnly) {
      return false;
    }
  }

  // Domain options check
  if (domainList && contextDomain) {
    if (!contextDomainMatchesFilter(contextDomain)) {
      return false;
    }
  }

  // If we're in the context of third-party site, then consider
  // third-party option checks
  if (context & (FOThirdParty | FONotThirdParty)) {
    if ((filterOption & FOThirdParty) &&
        (context & FONotThirdParty)) {
      return false;
    }
    if ((antiFilterOption & FOThirdParty) &&
        (context & FOThirdParty)) {
      return false;
    }
  }

  return true;
}


const char * getNextPos(const char *input, char separator, const char *end) {
  const char *p = input;
  while (p != end && *p != '\0' && *p != separator) {
    p++;
  }
  return p;
}

/**
 * Similar to str1.indexOf(filter, startingPos) but with
 * extra consideration to some ABP filter rules like ^.
 */
int indexOfFilter(const char* input, int inputLen,
                  const char* filterBegin, const char *filterEnd) {
  const int filterLen = filterEnd - filterBegin;
  if (1 == filterLen && '^' == *filterBegin) return -1;
  if (filterLen > inputLen) {
    return -1;
  }

  for (int i = 0; i < inputLen; ++i) {
    bool match = true;
    for (int j = 0; j < filterLen; ++j) {
      const char inputChar = input[i+j];
      const char filterChar = filterBegin[j];

      if (filterChar != inputChar) {
        // ^abc^ matches both /abc/ and /abc
        if ('^' == filterChar &&
            (isSeparatorChar(inputChar) || '\0' == inputChar)) {
          continue;
        }
        if ('\0' == inputChar) {
          return -1;
        }
        match = false;
        break;
      }
    }
    if (match) {
      return i;
    }
  }
  return -1;
}

bool Filter::matches(const char *input, FilterOption contextOption,
    const char *contextDomain, BloomFilter *inputBloomFilter,
    const char *inputHost, int inputHostLen) {
  return matches(input, static_cast<int>(strlen(input)), contextOption,
      contextDomain, inputBloomFilter, inputHost, inputHostLen);
}

bool Filter::matches(const char *input, int inputLen,
    FilterOption contextOption, const char *contextDomain,
    BloomFilter *inputBloomFilter, const char *inputHost, int inputHostLen) {
  if (!matchesOptions(input, contextOption, contextDomain)) {
    return false;
  }

  if (!data) {
    return false;
  }

  // We lazily figure out the dataLen only once
  if (dataLen == -1) {
    dataLen = static_cast<int>(strlen(data));
  }

  // Check for a regex match
  if (filterType & FTRegex) {
#ifdef ENABLE_REGEX
    std::smatch m;
    std::regex e(data, std::regex_constants::extended);
    return std::regex_search(std::string(input), m, e);
#else
    return false;
#endif
  }

  // Check for both left and right anchored
  if ((filterType & FTLeftAnchored) && (filterType & FTRightAnchored)) {
    return !strcmp(data, input);
  }

  // Check for right anchored
  if (filterType & FTRightAnchored) {
    if (dataLen > inputLen) {
      return false;
    }

    return !strcmp(input + (inputLen - dataLen), data);
  }

  // Check for left anchored
  if (filterType & FTLeftAnchored) {
    return !strncmp(data, input, dataLen);
  }

  // Check for domain name anchored
  if (filterType & FTHostAnchored) {
    int currentHostLen = inputHostLen;
    const char *currentHost = inputHost;
    if (!currentHostLen) {
      currentHost = getUrlHost(input, &currentHostLen);
    }
    int hostLen = 0;
    if (host) {
      hostLen = this->hostLen == -1 ?
        static_cast<int>(strlen(host)) : this->hostLen;
    }

    if (inputBloomFilter) {
      for (int i = 1; i < hostLen; i++) {
        if (!inputBloomFilter->exists(host + i - 1, 2)) {
          return false;
        }
      }
    }

    if (isThirdPartyHost(host, hostLen, currentHost, currentHostLen)) {
      return false;
    }
  }

  // Wildcard match comparison
  const char *filterPartStart = data;
  const char *filterPartEnd = getNextPos(data, '*', data + dataLen);
  int index = 0;
  while (filterPartStart != filterPartEnd || *filterPartStart == '*') {
    int filterPartLen = static_cast<int>(filterPartEnd - filterPartStart);

    if (inputBloomFilter) {
      for (int i = 1; i < filterPartLen && filterPartEnd -
          filterPartStart - i >= 2; i++) {
        if (!isSeparatorChar(*(filterPartStart + i - 1)) &&
            !isSeparatorChar(*(filterPartStart + i)) &&
            !inputBloomFilter->exists(filterPartStart + i - 1, 2)) {
          return false;
        }
      }
    }

    int newIndex = indexOfFilter(input + index, inputLen - index,
        filterPartStart, filterPartEnd);
    if (newIndex == -1) {
      return false;
    }
    newIndex += index;

    if (*filterPartEnd == '\0' || filterPartEnd == data + dataLen) {
      break;
    }
    const char *temp = getNextPos(filterPartEnd + 1, '*', data + dataLen);
    filterPartStart = filterPartEnd + 1;
    filterPartEnd = temp;
    index = newIndex + filterPartLen;
    if (*(input + newIndex) == '\0') {
      break;
    }
  }

  return true;
}

void Filter::parseDomains(const char* domainList) {
  if (!domainList || domainsParsed) {
    return;
  }
  int startOffset = 0;
  int len = 0;
  const char* p = domainList;
  while (true) {
    if (*p == '|' || *p == '\0') {
      const char *domain = domainList + startOffset;
      if (*domain == '~') {
        cout << "about to insert: " << std::string(domain + 1, len - 1) << endl;
        antiDomains.insert(std::string(domain + 1, len - 1));
        cout << "inserted" << endl;
      } else {
        cout << "2.about to insert: " << std::string(domain, len) << endl;
        domains.insert(std::string(domain, len));
        cout << "2.inserted" << endl;
      }
      startOffset += len + 1;
      len = -1;
    }
    if (*p == '\0') {
      break;
    }
    p++;
    len++;
  }
  domainsParsed = true;
}

uint64_t Filter::hash() const {
  if (!host && !data) {
    return 0;
  } else if (host) {
    return h(host, hostLen == -1 ? static_cast<int>(strlen(host)) : hostLen);
  }

  return h(data, dataLen);
}

uint32_t Filter::Serialize(char *buffer) {
  uint32_t totalSize = 0;
  char sz[64];
  uint32_t dataLenSize = 1 + snprintf(sz, sizeof(sz),
      "%x,%x,%x,%x", dataLen, filterType,
      filterOption, antiFilterOption);
  if (buffer) {
    memcpy(buffer + totalSize, sz, dataLenSize);
  }
  totalSize += dataLenSize;
  if (buffer) {
    memcpy(buffer + totalSize, data, dataLen);
  }
  totalSize += dataLen;

  if (host) {
    int hostLen = this->hostLen == -1 ?
      static_cast<int>(strlen(host)) : this->hostLen;
    if (buffer) {
      memcpy(buffer + totalSize, host, hostLen + 1);
    }
    totalSize += hostLen;
  }
  totalSize += 1;

  if (domainList) {
    int domainListLen = static_cast<int>(strlen(domainList));
    if (buffer) {
      memcpy(buffer + totalSize, domainList, domainListLen + 1);
    }
    totalSize += domainListLen;
  }
  totalSize += 1;

  return totalSize;
}

bool hasNewlineBefore(char *buffer, uint32_t bufferSize) {
  char *p = buffer;
  for (uint32_t i = 0; i < bufferSize; ++i) {
    if (*p == '\0')
      return true;
    p++;
  }
  return false;
}

uint32_t Filter::Deserialize(char *buffer, uint32_t bufferSize) {
  dataLen = 0;
  if (!hasNewlineBefore(buffer, bufferSize)) {
    return 0;
  }
  sscanf(buffer, "%x,%x,%x,%x", &dataLen, (unsigned int*)&filterType,
      (unsigned int*)&filterOption, (unsigned int*)&antiFilterOption);
  uint32_t consumed = static_cast<uint32_t>(strlen(buffer)) + 1;
  if (consumed + dataLen >= bufferSize) {
    return 0;
  }

  data = buffer + consumed;
  consumed += dataLen;

  uint32_t hostLen = static_cast<uint32_t>(strlen(buffer + consumed));
  if (hostLen != 0) {
    host = buffer + consumed;
  } else {
    host = nullptr;
  }
  consumed += hostLen + 1;

  uint32_t domainListLen = static_cast<uint32_t>(strlen(buffer + consumed));
  if (domainListLen != 0) {
    domainList = buffer + consumed;
  } else {
    domainList = nullptr;
  }
  consumed += domainListLen + 1;

  borrowed_data = true;

  return consumed;
}
