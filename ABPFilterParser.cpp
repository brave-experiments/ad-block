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
  FPData,
};

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
