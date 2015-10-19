#include "ABPFilterParser.h"
#include <string.h>
#include <iostream>

using namespace std;

const int maxLineLength = 2048;
const char *separatorCharacters = ":?/=^";

enum FilterParseState {
  FPStart,
  FPPastWhitespace,
  FPOneBar,
  FPOneAt,
  FPData
};

bool isSeparatorChar(char c) {
  const char *p = separatorCharacters;
  while (*p != 0) {
    if (*p == c) {
      return true;
    }
    ++p;
  };
  return false;
}

int findFirstSeparatorChar(const char *input) {
  const char *p = input;
  while (*p != '\0') {
    if (isSeparatorChar(*p)) {
      return p - input;
    }
    p++;
  }
  return -1;
}

/*
bool matches(const char *input) {
  if (hasMatchingFilters(parserData.filters, parserData, input, contextParams, cachedInputData) ||
      hasMatchingNoFingerprintFilters === true || hasMatchingNoFingerprintFilters === undefined &&
      hasMatchingFilters(parserData.noFingerprintFilters, parserData, input, contextParams, cachedInputData)) {

    // Check for exceptions only when there's a match because matches are
    // rare compared to the volume of checks
    let exceptionBloomFilterMiss = parserData.exceptionBloomFilter && !parserData.exceptionBloomFilter.substringExists(cleanedInput, fingerprintSize);
    if (!exceptionBloomFilterMiss || hasMatchingFilters(parserData.exceptionFilters, parserData, input, contextParams, cachedInputData)) {
      return false;
    }

    return true;
  }

  return false;
}
*/

// Not currently multithreaded safe due to the static buffer named 'data'
void parseFilter(const char *input, Filter &f) {
  FilterParseState parseState = FPStart;
  const char *p = input;
  static char data[1024];
  memset(data, 0, sizeof data);
  int i = 0;
  while (*p != '\0') {
    // Check for the filter being too long
    if ((p - input) >= maxLineLength - 1) {
      return;
    }

    if (parseState == FPOneBar && *p != '|') {
      parseState = FPData;
      f.filterType = FTLeftAnchored;
    }

    switch (*p) {
      case '|':
        if (parseState == FPStart || parseState == FPPastWhitespace) {
          parseState = FPOneBar;
          p++;
          continue;
        } else if (parseState == FPOneBar) {
          parseState = FPOneBar;
          f.filterType = FTHostAnchored;
          parseState = FPData;
          p++;
          int len = findFirstSeparatorChar(p);
          f.domainList = new char[len + 1];
          f.domainList[len] = '\0';
          memcpy(f.domainList, p, len);
          continue;
        } else {
          f.filterType = static_cast<FilterType>(f.filterType | FTRightAnchored);
          parseState = FPData;
          p++;
          continue;
        }
        break;
      case '@':
        if (parseState == FPStart || parseState == FPPastWhitespace) {
          parseState = FPOneAt;
          p++;
          continue;
        } else if (parseState == FPOneAt) {
          parseState = FPOneBar;
          f.filterType = FTException;
          parseState = FPData;
          p++;
          continue;
        }
        break;
      case '!':
      case '[':
        if (parseState == FPStart || parseState == FPPastWhitespace) {
          f.filterType = FTComment;
          // Wed don't care about comments right now
          return;
        }
        break;
      case '\r':
      case '\n':
      case '\t':
      case ' ':
        // Skip leading whitespace
        if (parseState == FPStart) {
          p++;
          continue;
        }
        break;
      case '/':
        if ((parseState == FPStart || parseState == FPPastWhitespace) && input[strlen(input) -1] == '/') {
          // Just copy out the whole regex and return early
          int len = strlen(input) - i - 1;
          f.data = new char[len];
          f.data[len - 1] = '\0';
          memcpy(f.data, input + i + 1, len - 1);
          f.filterType = FTRegex;
          return;
        }
        break;
      case '$':
        f.parseOptions(p + 1);
        data[i] = '\0';
        f.data = new char[i + 1];
        memcpy(f.data, data, i + 1);
        return;
      case '#':
        if (*(p+1) == '#') {
          // TODO
          f.filterType = FTElementHiding;
          return;
        } else if (*(p+1) == '@') {
          f.filterType = FTElementHidingException;
          return;
        }
      default:
        parseState = FPData;
        break;
    }

    data[i] = *p;
    i++;
    p++;
  }

  data[i] = '\0';
  f.data = new char[i];
  memcpy(f.data, data, i + 1);
}


ABPFilterParser::ABPFilterParser() : filters(nullptr),
  htmlRuleFilters(nullptr),
  exceptionFilters(nullptr),
  noFingerprintFilters(nullptr),
  numFilters(0),
  numHtmlRuleFilters(0),
  numExceptionFilters(0),
  numNoFingerprintFilters(0) {
}

ABPFilterParser::~ABPFilterParser() {
  if (filters) {
    delete[] filters;
  }
  if( htmlRuleFilters) {
    delete[] htmlRuleFilters;
  }
  if (exceptionFilters) {
    delete[] exceptionFilters;
  }
  if (noFingerprintFilters) {
   delete[] noFingerprintFilters;
  }
}

// Parses the filter data into a few collections of filters and enables efficent querying
bool ABPFilterParser::parse(const char *input) {
  const char *p = input;
  const char *lineStart = p;

  // Parsing does 2 passes, one just to determine the type of information we'll need to setup.
  while (*p != '\0') {
    if (*p == '\n') {
      Filter f;
      parseFilter(lineStart, f);


    }

    p++;
  };


  return true;
}

