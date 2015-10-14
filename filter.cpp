#include "filter.h"

Filter::Filter() :
  filterType(noFilterType),
  filterOption(noFilterOption),
  antiFilterOption(noFilterOption),
  data(nullptr) {
}

Filter::~Filter() {
  if (data) {
    delete[] data;
  }
}
