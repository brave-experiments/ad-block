#include "ABPFilterParser.h"
#include <string.h>

const char *separatorCharacters = ":?/=^";
const int maxLineLength = 2048;

enum FilterParseState {
  FPStart,
  FPPastWhitespace,
};

// Not currently multithreaded safe due to the static buffer named 'data'
void parseFilter(const char *input, Filter &f) {
  FilterParseState parseState = FPStart;
  const char *p = input;
  static char data[1024];
  int i = 0;
  while (*p != '\0') {

    // Check for the filter being too long
    if (i >= maxLineLength - 1) {
      return;
    }

    switch (*p) {
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
    }

    data[i] = *p;
    i++;
    p++;
  }

  data[i] = '\0';
  f.data = new char[i];
  memcpy(f.data, data, i);
}
