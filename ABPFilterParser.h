#pragma once

#include "filter.h"

class ABPFilterParser
{
public:
  ABPFilterParser();
  ~ABPFilterParser();

  bool parse(const char *input);
  bool matches(const char *input);

  Filter *filters;
  Filter *htmlRuleFilters;
  Filter *exceptionFilters;
  Filter *noFingerprintFilters;
  int numFilters;
  int numHtmlRuleFilters;
  int numExceptionFilters;
  int numNoFingerprintFilters;

protected:
  bool hasMatchingFilters(Filter *filter, int &numFilters, const char *input);

};

extern const char *separatorCharacters;
void parseDomains(const char *input);
int findFirstSeparatorChar(const char *input, int startingPos);
void parseHTMLFilter(const char *input, int index, Filter *);
void parseFilter(const char *input, const char *end, Filter&);
void parseFilter(const char *input, Filter&);
int getDomainIndex(const char *input);
void getUrlHost(const char *input, char *hostBuffer);
bool filterDataContainsOption(Filter *);
bool isThirdPartyHost(const char *input);
void getFingerprint(const char *input);
bool isSeparatorChar(char c);
int findFirstSeparatorChar(const char *input);
