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

enum ElementType {
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

enum FilterOption {
  FONoFilterOption = 0,
  FOScript = 01,
  FOImage = 02,
  FOStylesheet = 04,
  FOObject = 010,
  FOXmlHttpRequest = 020,
  FOObjectSubrequest = 040,
  FOSubdocument = 0100,
  FODocument = 0200,
  FOThirdParty = 01000,
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
