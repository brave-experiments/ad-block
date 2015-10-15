#include "ABPFilterParser.h"
#include <string.h>
#include <iostream>

using namespace std;

const char *separatorCharacters = ":?/=^";
const int maxLineLength = 2048;

enum FilterParseState {
  FPStart,
  FPPastWhitespace,
  FPOneBar,
  FPData
};

void parseOption(const char *input, int len, FilterOption *filterOption, FilterOption *antiFilterOption) {
  FilterOption *pFilterOption = filterOption;
  const char *pStart = input;
  if (input[0] == '~') {
    pFilterOption = antiFilterOption;
    pStart++;
    len --;
  }

  if (!memcmp(pStart, "script", len)) {
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
  } else if (!memcmp(pStart, "other", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOOther);
  } else if (!memcmp(pStart, "elemhide", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOElemHide);
  } else if (!memcmp(pStart, "third-party", len)) {
    *pFilterOption = static_cast<FilterOption>(*pFilterOption | FOThirdParty);
  }
  // Otherwise just ignore the option, maybe something new we don't support yet
}

void parseOptions(const char *input, FilterOption *filterOption, FilterOption *antiFilterOption) {
  int start = 0;
  int len = 0;
  const char *p = input;
  while (*p != '\0') {
    if (*p == ',') {
      parseOption(input + start, len, filterOption, antiFilterOption);
      start = len + 1;
      len = -1;
    }
    p++;
    len++;
  }
  parseOption(input + start, len, filterOption, antiFilterOption);
}

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
      f.filterType = leftAnchored;
    }

    switch (*p) {
      case '|':
        if (parseState == FPStart || parseState == FPPastWhitespace) {
          parseState = FPOneBar;
          p++;
          continue;
        } else if (parseState == FPOneBar) {
          parseState = FPOneBar;
          f.filterType = hostAnchored;
          parseState = FPData;
          p++;
          continue;
        } else {
          f.filterType = static_cast<FilterType>(f.filterType | rightAnchored);
          parseState = FPData;
          p++;
          continue;
        }
        break;
      case '!':
      case '[':
        if (parseState == FPStart || parseState == FPPastWhitespace) {
          f.filterType = comment;
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
          f.filterType = regex;
          return;
        }
        break;
      case '$':
        parseOptions(p + 1, &f.filterOption, &f.antiFilterOption);
        data[i] = '\0';
        f.data = new char[i];
        memcpy(f.data, data, i + 1);
        return;
      case '#':
        if (*(p+1) == '#') {
          // TODO
          f.filterType = elementHiding;
          return;
        } else if (*(p+1) == '@') {
          f.filterType = elementHidingException;
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
