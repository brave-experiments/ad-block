/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ABPFilterParserWrap.h"

namespace ABPFilterParserWrap {

using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Boolean;
using v8::Value;

Persistent<Function> ABPFilterParserWrap::constructor;

ABPFilterParserWrap::ABPFilterParserWrap() {
}

ABPFilterParserWrap::~ABPFilterParserWrap() {
}

void ABPFilterParserWrap::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "ABPFilterParser"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "parse", ABPFilterParserWrap::Parse);
  NODE_SET_PROTOTYPE_METHOD(tpl, "matches", ABPFilterParserWrap::Matches);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "ABPFilterParser"),
               tpl->GetFunction());
}

void ABPFilterParserWrap::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new ABPFilterParser(...)`
    ABPFilterParserWrap* obj = new ABPFilterParserWrap();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `ABPFilterParser(...)`,
    // turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void ABPFilterParserWrap::Parse(const FunctionCallbackInfo<Value>& args) {
  String::Utf8Value str(args[0]->ToString());
  const char * buffer = *str;

  ABPFilterParserWrap* obj =
    ObjectWrap::Unwrap<ABPFilterParserWrap>(args.Holder());
  obj->parse(buffer);
}

void ABPFilterParserWrap::Matches(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(args[0]->ToString());
  const char * buffer = *str;

  ABPFilterParserWrap* obj =
    ObjectWrap::Unwrap<ABPFilterParserWrap>(args.Holder());
  bool matches = obj->matches(buffer);
  args.GetReturnValue().Set(Boolean::New(isolate, matches));
}


}  // namespace ABPFilterParserWrap
