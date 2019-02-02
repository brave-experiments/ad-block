/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ETLD_SERIALIZATION_H_
#define ETLD_SERIALIZATION_H_

#include <string>
#include <vector>

namespace brave_etld {

typedef std::string SerializedBuffer;
typedef std::vector<SerializedBuffer> SerializedChildBuffers;

struct SerializedBufferInfo {
  size_t body_len;
  size_t body_start;
  size_t buffer_len;
};

struct SerializationResult {
  SerializedBuffer buffer;
  size_t body_len;
  size_t body_start;
};

SerializedBufferInfo extract_buffer_info(const SerializedBuffer &buffer);
SerializedChildBuffers deserialize_buffer(const SerializedBuffer &buffer);

}  // namespace brave_etld

#endif  // ETLD_SERIALIZATION_H_
