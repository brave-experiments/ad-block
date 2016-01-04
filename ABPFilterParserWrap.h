/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ABPFILTERPARSERWRAP_H_
#define ABPFILTERPARSERWRAP_H_

#include <node.h>
#include <node_object_wrap.h>

#include "ABPFilterParser.h"

namespace ABPFilterParserWrap {

/**
 * Wraps Bloom Filter for use in Node
 */
class ABPFilterParserWrap : public ABPFilterParser, public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  ABPFilterParserWrap();
  virtual ~ABPFilterParserWrap();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

  static void Parse(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Matches(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Serialize(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Deserialize(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Cleanup(const v8::FunctionCallbackInfo<v8::Value>& args);

  static v8::Persistent<v8::Function> constructor;
};

}  // namespace ABPFilterParserWrap

#endif  // ABPFILTERPARSERWRAP_H_
