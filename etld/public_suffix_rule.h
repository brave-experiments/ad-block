/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_ETLD_PUBLIC_SUFFIX_RULE_H_
#define BRAVE_ETLD_PUBLIC_SUFFIX_RULE_H_

#include <string>
#include <vector>
#include "./domain.h"
#include "./types.h"

namespace Brave {
namespace eTLD {

class PublicSuffixRule {
  public:
    PublicSuffixRule(const std::string &raw_text);

    bool matches(const Domain &domain) const;
    bool isException() const {
      return is_exception_;
    };
    bool isWildcard() const {
      return is_wildcard_;
    }

  protected:
    void parseRule(const std::string &rule_text);

    std::vector<Label> labels_;
    bool is_exception_;
    bool is_wildcard_;
};

}
}

#endif