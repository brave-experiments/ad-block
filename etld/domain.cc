/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include "./domain.h"

namespace Brave {
namespace eTLD {

std::string Domain::toString() const {
  std::stringstream asString;
  for (auto &&str : labels_) {
    asString << str;
  }
  return asString.str();
}

}
}