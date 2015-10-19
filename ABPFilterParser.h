#pragma once

#include "filter.h"

class ABPFilterParser
{
public:
  ABPFilterParser();
  ~ABPFilterParser();

  bool parse(const char *input);

  const Filter *filters;
  const Filter *htmlRuleFilters;
  const Filter *exceptionFilters;
  const Filter *noFingerprintFilters;
  int numFilters;
  int numHtmlRuleFilters;
  int numExceptionFilters;
  int numNoFingerprintFilters;
};

extern const char *separatorCharacters;
void parseDomains(const char *input);
int findFirstSeparatorChar(const char *input, int startingPos);
void parseHTMLFilter(const char *input, int index, Filter *);
void parseFilter(const char *input, Filter&);
int getDomainIndex(const char *input);
void getUrlHost(const char *input, char *hostBuffer);
bool filterDataContainsOption(Filter *);
bool isThirdPartyHost(const char *input);
bool matches(const char *input);
void getFingerprint(const char *input);
bool isSeparatorChar(char c);
int findFirstSeparatorChar(const char *input);


