#include "filter.h"

Filter::Filter() :
  filterType(noFilterType),
  filterOption(FONoFilterOption),
  antiFilterOption(FONoFilterOption),
  data(nullptr) {
}

Filter::~Filter() {
  if (data) {
    delete[] data;
  }
}
