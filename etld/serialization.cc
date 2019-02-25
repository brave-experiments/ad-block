/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "etld/serialization.h"
#include <vector>
#include <string>

namespace brave_etld {
namespace internal {

SerializedBufferInfo extract_buffer_info(const SerializedBuffer& buffer) {
  const size_t splitter_pos = buffer.find(":");
  if (splitter_pos == std::string::npos) {
    return {
      std::string::npos,
      std::string::npos,
      0
    };
  }

  const std::string header_str = buffer.substr(0, splitter_pos);
  const size_t body_len = std::stoi(header_str);
  const size_t body_start = splitter_pos + 1;
  const size_t buffer_len = body_start + body_len;
  return {
    body_len,
    body_start,
    buffer_len
  };
}

SerializedChildBuffers deserialize_buffer(const SerializedBuffer& buffer) {
  std::vector<SerializedBuffer> child_buffers;
  const SerializedBufferInfo info = extract_buffer_info(buffer);
  if (info.body_len == std::string::npos) {
    return child_buffers;
  }

  const size_t payload_len = buffer.size();
  size_t offset = info.body_start;
  std::string child_buffer;
  while (offset < payload_len) {
    const SerializedBufferInfo child_info = extract_buffer_info(
      buffer.substr(offset, payload_len - offset));
    child_buffer = buffer.substr(offset, child_info.buffer_len);
    child_buffers.push_back(child_buffer);
    offset += child_info.buffer_len;
  }

  return child_buffers;
}

}  // namespace internal
}  // namespace brave_etld
