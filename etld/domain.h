/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_DOMAIN_H_
#define ETLD_DOMAIN_H_

#include <string>
#include <vector>
#include "etld/types.h"

using ::std::vector;
using ::std::string;

namespace brave_etld {

class Domain {
 public:
  explicit Domain(const Domain& domain) = default;
  explicit Domain(const vector<Label>& labels);
  explicit Domain(const string& domain);
  ~Domain() = default;

  bool Equals(const Domain &domain);

  size_t Length() const {
    return labels_.size();
  }

  const Label& Get(size_t idx) const {
    return labels_[idx];
  }

  string ToString() const;

 protected:
  vector<Label> labels_;
};

}  // namespace brave_etld

#endif  // ETLD_DOMAIN_H_
