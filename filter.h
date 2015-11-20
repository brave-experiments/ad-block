#pragma once

#include "base.h"
#include <stdint.h>
#include <string.h>

enum FilterType {
  FTNoFilterType = 0,
  FTRegex = 01,
  FTElementHiding = 02,
  FTElementHidingException = 04,
  FTHostAnchored = 010,
  FTLeftAnchored = 020,
  FTRightAnchored = 040,
  FTComment = 0100,
  FTException = 0200,
  FTEmpty = 0400,
  FTHostOnly = 01000,
  FTListTypesMask = FTException|FTElementHiding|FTElementHidingException|FTEmpty|FTComment,
};

enum FilterOption {
  FONoFilterOption = 0,
  FOScript = 01,
  FOImage = 02,
  FOStylesheet = 04,
  FOObject = 010,
  FOXmlHttpRequest = 020,
  FOObjectSubrequest = 040,
  FOSubdocument = 0100,
  FODocument = 0200,
  FOOther = 0400,
  FOXBL = 01000,
  FOCollapse = 02000,
  FODoNotTrack = 04000,
  FOElemHide = 010000,
  FOThirdParty = 020000,
  // both options because the not present case is reated as unknown
  FONotThirdParty = 040000
};

class Filter {
friend class ABPFilterParser;
public:
  Filter();
  Filter(const Filter &other);
  Filter(const char * data, int dataLen, char *domainList = nullptr, const char * host = nullptr) :
    borrowedData(true), filterType(FTNoFilterType), filterOption(FONoFilterOption),
    antiFilterOption(FONoFilterOption), data(const_cast<char*>(data)), dataLen(dataLen), domainList(domainList), host(const_cast<char*>(host)) {
  }
  Filter(FilterType filterType, FilterOption filterOption,FilterOption antiFilterOption,
         const char * data, int dataLen, char *domainList = nullptr, const char * host = nullptr) :
    borrowedData(true), filterType(filterType), filterOption(filterOption),
    antiFilterOption(antiFilterOption), data(const_cast<char*>(data)), dataLen(dataLen), domainList(domainList), host(const_cast<char *>(host)) {
  }

  ~Filter();

  // Swaps the data members for 'this' and the passed in filter
  void swap(Filter&f);

  // Checks to see if any filter matches the input but does not match any exception rule
  // You may want to call the first overload to be slighly more efficient
  bool matches(const char *input, int inputLen, FilterOption contextOption = FONoFilterOption, const char *contextDomain = nullptr);
  bool matches(const char *input, FilterOption contextOption = FONoFilterOption, const char *contextDomain = nullptr);

  // Nothing needs to be updated when a filter is added multiple times
  void update(const Filter &) {}

  // Checks to see if the filter options match for the passed in data
  bool matchesOptions(const char *input, FilterOption contextOption, const char *contextDomain = nullptr);

  void parseOptions(const char *input);
  bool containsDomain(const char *domain, bool anti = false) const;
  int getDomainCount(bool anti = false) const;

  uint64_t hash() const;

  bool operator==(const Filter &rhs) const {
    /*
     if (filterType != rhs.filterType || filterOption != rhs.filterOption || antiFilterOption != rhs.antiFilterOption) {
      return false;
    }
    */

    int hostLen = strlen(host);
    int rhsHostLen = strlen(rhs.host);

    if (hostLen != rhsHostLen) {
      return false;
    }

    return !memcmp(host, rhs.host, hostLen);
  }

  bool operator!=(const Filter &rhs) const {
    return !(*this == rhs);
  }

  uint32_t serialize(char *buffer);
  uint32_t deserialize(char *buffer, uint32_t bufferSize);

  // Holds true if the filter should not free memory because for example it was loaded
  // from a large buffer somewhere else via the serialize and deserialize functions.
  bool borrowedData;

  FilterType filterType;
  FilterOption filterOption;
  FilterOption antiFilterOption;
  char *data;
  int dataLen;
  char *domainList;
  char *host;

protected:
  // Filters the domain list down to what's applicable for the context domain
  void filterDomainList(const char *domainList, char *destBuffer, const char *contextDomain, bool anti);
  // Checks for what is not excluded by the opposite list
  int getLeftoverDomainCount(const char *shouldBlockDomains, const char *shouldSkipDomains);

  // Parses a single option
  void parseOption(const char *input, int len);
};
