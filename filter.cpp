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
