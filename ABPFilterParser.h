/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ABPFILTERPARSER_H_
#define ABPFILTERPARSER_H_

#include "./filter.h"
#include "BloomFilter.h"
#include "HashSet.h"
#include "./badFingerprint.h"
#include "./cosmeticFilter.h"

class ABPFilterParser {
 public:
  ABPFilterParser();
  ~ABPFilterParser();

  bool parse(const char *input);
  bool matches(const char *input, FilterOption contextOption = FONoFilterOption,
      const char *contextDomain = nullptr);
  // Serializes a the parsed data and bloom filter data into a single buffer.
  // The returned buffer should be deleted.
  char * serialize(int *size, bool ignoreHTMLFilters = true);
  // Deserializes the buffer, a size is not needed since a serialized
  // buffer is self described
  void deserialize(char *buffer);

  void enableBadFingerprintDetection();

  Filter *filters;
  Filter *htmlRuleFilters;
  Filter *exceptionFilters;
  Filter *noFingerprintFilters;
  Filter *noFingerprintExceptionFilters;

  int numFilters;
  int numHtmlRuleFilters;
  int numExceptionFilters;
  int numNoFingerprintFilters;
  int numNoFingerprintExceptionFilters;
  int numHostAnchoredFilters;
  int numHostAnchoredExceptionFilters;

  BloomFilter *bloomFilter;
  BloomFilter *exceptionBloomFilter;
  HashSet<Filter> *hostAnchoredHashSet;
  HashSet<Filter> *hostAnchoredExceptionHashSet;

  // Used only in the perf program to create a list of bad fingerprints
  BadFingerprintsHashSet *badFingerprintsHashSet;

  // Stats kept for matching
  unsigned int numFalsePositives;
  unsigned int numExceptionFalsePositives;
  unsigned int numBloomFilterSaves;
  unsigned int numExceptionBloomFilterSaves;

 protected:
  // Determines if a passed in array of filter pointers matches for any of
  // the input
  bool hasMatchingFilters(Filter *filter, int numFilters, const char *input,
      int inputLen, FilterOption contextOption, const char *contextDomain,
      BloomFilter *inputBloomFilter, const char *inputHost, int inputHostLen);
  void initBloomFilter(BloomFilter**, const char *buffer, int len);
  void initHashSet(HashSet<Filter>**, char *buffer, int len);
};

// Fast hash function applicable to 2 byte char checks
class HashFn2Byte : public HashFn {
 public:
  HashFn2Byte() : HashFn(0, false) {
  }

  uint64_t operator()(const char *input, int len,
      unsigned char lastCharCode, uint64_t lastHash) override;

  uint64_t operator()(const char *input, int len) override;
};

extern const char *separatorCharacters;
void parseFilter(const char *input, const char *end, Filter *f,
    BloomFilter *bloomFilter = nullptr,
    BloomFilter *exceptionBloomFilter = nullptr,
    HashSet<Filter> *hostAnchoredHashSet = nullptr,
    HashSet<Filter> *hostAnchoredExceptionHashSet = nullptr,
    HashSet<CosmeticFilter> *simpleCosmeticFilters = nullptr);
void parseFilter(const char *input, Filter *f,
    BloomFilter *bloomFilter = nullptr,
    BloomFilter *exceptionBloomFilter = nullptr,
    HashSet<Filter> *hostAnchoredHashSet = nullptr,
    HashSet<Filter> *hostAnchoredExceptionHashSet = nullptr,
    HashSet<CosmeticFilter> *simpleCosmeticFilters = nullptr);
bool isSeparatorChar(char c);
int findFirstSeparatorChar(const char *input, const char *end);

#endif  // ABPFILTERPARSER_H_
