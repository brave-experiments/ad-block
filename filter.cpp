#include "filter.h"
#include <string.h>
#include <iostream>

using namespace std;

Filter::Filter() :
  filterType(noFilterType),
  filterOption(FONoFilterOption),
  antiFilterOption(FONoFilterOption),
  data(nullptr),
  domainList(nullptr) {
}

Filter::~Filter() {
  if (data) {
    delete[] data;
  }
  if (domainList) {
    delete[] domainList;
  }
}


static const char *separatorCharacters = ":?/=^";
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

bool Filter::containsDomain(const char *domain, bool anti) const {
  if (!domainList) {
    return false;
  }

  int startOffset = 0;
  int len = 0;
  const char *p = domainList;
  while (*p != '\0') {
    if (*p == '|') {
      if (isDomain(domainList + startOffset, len, domain, anti)) {
        return true;
      }
      startOffset += len + 1;
      len = -1;
    }
    p++;
    len++;
  }
  return isDomain(domainList + startOffset, len, domain, anti);
}

int Filter::getDomainCount(bool anti) const {
  if (!domainList || domainList[0] == '\0') {
    return 0;
  }

  int count = 0;
  int startOffset = 0;
  int len = 0;
  const char *p = domainList;
  while (*p != '\0') {
    if (*p == '|') {
      if (*(domainList + startOffset) == '~' && anti) {
        count++;
      }
      else if (*(domainList + startOffset) != '~' && !anti) {
        count++;
      }
      startOffset = len + 1;
      len = -1;
    }
    p++;
    len++;
  }

  if (*(domainList + startOffset) == '~' && anti) {
    count++;
  }
  else if (*(domainList + startOffset) != '~' && !anti) {
    count++;
  }
  return count;
}

void Filter::parseOption(const char *input, int len) {
  FilterOption *pFilterOption = &filterOption;
  const char *pStart = input;
  if (input[0] == '~') {
    pFilterOption = &antiFilterOption;
    pStart++;
    len --;
  }

  if (len >= 7 && !memcmp(pStart, "domain=", 7)) {
    len -= 7;
    domainList = new char[len + 1];
    domainList[len] = '\0';
    memcpy(domainList, pStart + 7, len);
  } else if (!memcmp(pStart, "script", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOScript);
  } else if (!memcmp(pStart, "image", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOImage);
  } else if (!memcmp(pStart, "stylesheet", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOStylesheet);
  } else if (!memcmp(pStart, "object", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOObject);
  } else if (!memcmp(pStart, "xmlhttprequest", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOXmlHttpRequest);
  } else if (!memcmp(pStart, "object-subrequest", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOObjectSubrequest);
  } else if (!memcmp(pStart, "subdocument", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOSubdocument);
  } else if (!memcmp(pStart, "document", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FODocument);
  } else if (!memcmp(pStart, "xbl", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOXBL);
  } else if (!memcmp(pStart, "collapse", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOCollapse);
  } else if (!memcmp(pStart, "donottrack", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FODoNotTrack);
  } else if (!memcmp(pStart, "other", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOOther);
  } else if (!memcmp(pStart, "elemhide", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOElemHide);
  } else if (!memcmp(pStart, "third-party", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOThirdParty);
  }
  // Otherwise just ignore the option, maybe something new we don't support yet
}

void Filter::parseOptions(const char *input) {
  filterOption = FONoFilterOption;
  antiFilterOption = FONoFilterOption;
  int startOffset = 0;
  int len = 0;
  const char *p = input;
  while (*p != '\0') {
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

// Determines if there's a match based on the options, this doesn't
// mean that the filter rule should be accepted, just that the filter rule
// should be considered given the current context.
// By specifying context params, you can filter out the number of rules which are
// considered.
bool Filter::matchesOptions(const char *input) {
  return true;
}


const char * getNextPos(const char *input, char separator) {
  const char *p = input;
  while (*p != '\0' && *p != separator) {
    p++;
  }
  return p;
}

int indexOf(const char *source, const char *filterPartStart, const char *filterPartEnd) {
  const char *s = source;
  const char *fStart = filterPartStart;
  const char *notCheckedSource = source;

  while (*s != '\0') {
    if (fStart == filterPartEnd) {
      return s - source - (filterPartEnd - filterPartStart);
    }
    if (*s != *fStart) {
      notCheckedSource++;
      s = notCheckedSource;
      fStart = filterPartStart;
      continue;
    }

    fStart++;
    s++;
  }

  if (fStart == filterPartEnd) {
    return s - source - (filterPartEnd - filterPartStart);
  }

  return -1;
}

/**
 * Similar to str1.indexOf(filter, startingPos) but with
 * extra consideration to some ABP filter rules like ^.
 */
int indexOfFilter(const char* input, const char *filterPosStart, const char *filterPosEnd) {
  bool prefixedSeparatorChar = false;
  int filterLen = filterPosEnd - filterPosStart;
  int inputLen = strlen(input);
  int index = 0;
  int beginIndex = -1;
  if (filterLen > inputLen) {
    return -1;
  }

  const char *filterPartStart = filterPosStart;
  const char *filterPartEnd = getNextPos(filterPosStart, '^');
  if (filterPartEnd - filterPosEnd > 0) {
    filterPartEnd = filterPosEnd;
  }

  while (*(input + index) != '\0') {
    if (filterPartStart == filterPartEnd) {
      prefixedSeparatorChar = true;
    }
    int lastIndex = index;
    index = indexOf(input + index, filterPartStart, filterPartEnd);
    if (index == -1) {
      return -1;
    }
    index += lastIndex;
    if (beginIndex == -1) {
      beginIndex = index;
    }

    index += (filterPartEnd - filterPartStart);

    if (prefixedSeparatorChar) {
      char testChar = *(input + index + (filterPartEnd - filterPartStart));
      if (!isSeparatorChar(testChar)) {
        return -1;
      }
    }

    if (*filterPartEnd == '\0') {
      break;
    }
    const char *temp = getNextPos(filterPartEnd + 1, '^');
    filterPartStart = filterPartEnd + 1;
    filterPartEnd = temp;
    prefixedSeparatorChar = false;
    if (filterPartEnd - filterPosEnd > 0) {
      break;
    }
  }

  return beginIndex;
}

bool Filter::matches(const char *input) {
  if (!matchesOptions(input)) {
    return false;
  }

  if (!data) {
    return false;
  }
  int dataLen = strlen(data);
  int inputLen = strlen(input);

  // Check for a regex match
  if (filterType & regex) {
    // TODO
    /*
    if (!parsedFilterData.regex) {
      parsedFilterData.regex = new RegExp(parsedFilterData.data);
    }
    return parsedFilterData.regex.test(input);
    */
    return false;
  }

  // Check for both left and right anchored
  if ((filterType & leftAnchored) && (filterType & rightAnchored)) {
    return !strcmp(data, input);
  }

  // Check for right anchored
  if (filterType & rightAnchored) {
    if (dataLen > inputLen) {
      return false;
    }

    return !strcmp(input + (inputLen - dataLen), data);
  }

  // Check for left anchored
  if (filterType & leftAnchored) {
    return !strncmp(data, input, dataLen);
  }

  // Check for domain name anchored
  if (filterType & hostAnchored) {
    /*
  TODO
    if (!cachedInputData.currentHost) {
      cachedInputData.currentHost = getUrlHost(input);
    }

    return !isThirdPartyHost(parsedFilterData.host, cachedInputData.currentHost) &&
      indexOfFilter(input, parsedFilterData.data) !== -1;
    */
  }

  // Wildcard match comparison
  const char *filterPartStart = data;
  const char *filterPartEnd = getNextPos(data, '*');
  int index = 0;
  while (filterPartStart != filterPartEnd) {
    int filterPartLen = filterPartEnd - filterPartStart;
    int newIndex = indexOfFilter(input + index, filterPartStart, filterPartEnd);
    if (newIndex == -1) {
      return false;
    }
    newIndex += index;

    if (*filterPartEnd == '\0') {
      break;
    }
    const char *temp = getNextPos(filterPartEnd + 1, '*');
    filterPartStart = filterPartEnd + 1;
    filterPartEnd = temp;
    index = newIndex + filterPartLen;
    if (*(input + newIndex) == '\0') {
      break;
    }
  }

  return true;
}
