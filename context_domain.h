/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CONTEXT_DOMAIN_H_
#define CONTEXT_DOMAIN_H_

#include <math.h>
#include <string.h>
#include "./hashFn.h"

static HashFn context_domain_hash(19);

class ContextDomain {
 public:
  uint64_t GetHash() const {
    return context_domain_hash(data_, data_len_);
  }

  ~ContextDomain() {
    if (data_) {
      delete[] data_;
    }
  }
  explicit ContextDomain(const char* data) {
    data_len_ = static_cast<uint32_t>(strlen(data));
    data_ = new char[data_len_];
    memcpy(data_, data, data_len_);
  }

  ContextDomain(const char* data, int data_len) {
    data_len_ = data_len;
    data_ = new char[data_len];
    memcpy(data_, data, data_len);
  }

  ContextDomain(const ContextDomain &rhs) {
    data_len_ = rhs.data_len_;
    data_ = new char[data_len_];
    memcpy(data_, rhs.data_, data_len_);
  }

  ContextDomain() : data_(nullptr), data_len_(0) {
  }

  bool operator==(const ContextDomain&rhs) const {
    if (data_len_ != rhs.data_len_) {
      return false;
    }
    return !memcmp(data_, rhs.data_, data_len_);
  }

  bool operator!=(const ContextDomain &rhs) const {
    return !(*this == rhs);
  }

  void Update(const ContextDomain &other) {
  }

  uint32_t Serialize(char* buffer) {
    return 0;
  }

  uint32_t Deserialize(char* buffer, uint32_t buffer_size) {
    return 0;
  }

 private:
  char* data_;
  uint32_t data_len_;
};

#endif  // CONTEXT_DOMAIN_H_
