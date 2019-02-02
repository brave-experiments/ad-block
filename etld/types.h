/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_TYPES_H_
#define ETLD_TYPES_H_

#include <string>
#include <vector>

namespace brave_etld {

typedef std::string Label;

struct DomainInfo {
  std::string tld;
  std::string domain;
  std::string subdomain;
};

}  // namespace brave_etld

#endif  // ETLD_TYPES_H_
