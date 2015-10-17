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
  FOOther = 0400,
  FOXBL = 01000,
  FOCollapse = 02000,
  FODoNotTrack = 04000,
  FOElemHide = 010000,
  FOThirdParty = 020000
};

class Filter {
public:
  Filter();
  ~Filter();
  bool matches(const char *input);
  bool matchesOptions(const char *input);
  void parseOption(const char *input, int len);
  void parseOptions(const char *input);
  bool containsDomain(const char *domain, bool anti = false) const;
  int getDomainCount(bool anti = false) const;

  FilterType filterType;
  FilterOption filterOption;
  FilterOption antiFilterOption;
  char *data;
  char *domainList;
};
