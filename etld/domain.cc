/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "etld/domain.h"
#include <string>
#include <sstream>

namespace brave_etld {

Domain::Domain(const Domain& domain) :
  labels_(domain.labels_) {}

Domain::Domain(const std::vector<Label>& labels) :
  labels_(labels) {}

Domain::Domain(const std::string& string) {
  size_t current, previous = 0;
  current = string.find(".");
  if (current == std::string::npos) {
    labels_.push_back(string);
    return;
  }

  while (current != std::string::npos) {
    labels_.push_back(string.substr(previous, current - previous));
    previous = current + 1;
    current = string.find(".", previous);
  }

  if (previous != 0) {
    labels_.push_back(string.substr(previous, current - previous));
  }
}

bool Domain::Equals(const Domain& domain) {
  const size_t local_length = Length();
  if (local_length != domain.Length()) {
    return false;
  }

  for (size_t i = 0; i < local_length; i += 1) {
    if (labels_[i].compare(domain.labels_[i]) != 0) {
      return false;
    }
  }

  return true;
}

std::string Domain::ToString() const {
  std::stringstream as_string;
  const size_t num_labels = labels_.size();
  size_t i = 0;
  for (const auto &str : labels_) {
    as_string << str;
    if (i != num_labels - 1) {
      as_string << ".";
    }
    i += 1;
  }
  return as_string.str();
}

}  // namespace brave_etld
