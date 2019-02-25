/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>
#include <vector>
#include <string>
#include "etld/serialization.h"

namespace brave_etld {
namespace internal {

SerializedBufferInfo extract_buffer_info(const SerializedBuffer &buffer) {
  SerializedBufferInfo info;
  const size_t splitter_pos = buffer.find(":");
  if (splitter_pos == std::string::npos) {
    info.body_len = std::string::npos;
    info.body_start = std::string::npos;
    info.buffer_len = 0;
    return info;
  }

  const std::string header_str = buffer.substr(0, splitter_pos);
  info.body_start = splitter_pos + 1;
  info.body_len = std::stoi(header_str);
  info.buffer_len = info.body_start + info.body_len;
  return info;
}

SerializedChildBuffers deserialize_buffer(const SerializedBuffer &buffer) {
  std::vector<SerializedBuffer> child_buffers;
  SerializedBufferInfo info = extract_buffer_info(buffer);
  if (info.body_len == std::string::npos) {
    return child_buffers;
  }

  const size_t payload_len = buffer.size();
  size_t offset = info.body_start;
  std::string child_buffer;
  while (offset < payload_len) {
    info = extract_buffer_info(buffer.substr(offset, payload_len - offset));
    child_buffer = buffer.substr(offset, info.buffer_len);
    child_buffers.push_back(child_buffer);
    offset += info.buffer_len;
  }

  return child_buffers;
}

}  // namespace internal
}  // namespace brave_etld
