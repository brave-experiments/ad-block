#pragma once

enum FilterType {
  noFilterType = 0,
  regex = 01,
  elementHiding = 02,
  elementHidingException = 04,
  hostAnchored = 010,
  leftAnchored = 020,
  rightAnchored = 040,
  comment = 0100,
};

enum FilterOption {
  noFilterOption = 0,
  script = 01,
  image = 02,
  stylesheet = 04,
  object = 010,
  xmlHttpRequest = 020,
  objectSubrequest = 040,
  subdocument = 0100,
  document = 0200,
  other = 0400,
};

class Filter {
public:
  Filter();
  ~Filter();

  FilterType filterType;
  FilterOption filterOption;
  FilterOption antiFilterOption;
  char *data;
};
