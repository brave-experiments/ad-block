/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_ETLD_DOMAIN_H_
#define BRAVE_ETLD_DOMAIN_H_

#include <string>
#include <sstream>
#include <vector>
#include "./types.h"

namespace Brave {
namespace eTLD {

class Domain {
  public:
    Domain(const std::string &domain) {
      std::size_t current, previous = 0;
      current = domain.find(".");
      while (current != std::string::npos) {
          labels_.push_back(domain.substr(previous, current - previous));
          previous = current + 1;
          current = domain.find(".", previous);
      }
      if (previous != 0) {
        labels_.push_back(domain.substr(previous, current - previous));
      }
    };

    int length() const {
      return labels_.size();
    }

    std::string toString() const;

  protected:
    std::vector<Label> labels_;
};

}
}

#endif