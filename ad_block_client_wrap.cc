/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "./ad_block_client_wrap.h"
#include <node_buffer.h>
#include <algorithm>
#include <fstream>
#include "./bad_fingerprint.h"
#include "./data_file_version.h"
#include "./filter.h"
#include "./filter_list.h"
#include "./lists/regions.h"
#include "./lists/malware.h"
#include "./lists/default.h"

namespace ad_block_client_wrap {

using v8::Array;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Int32;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Boolean;
using v8::Value;
using v8::Exception;
using v8::NewStringType;

#if V8_MAJOR_VERSION >= 7
#define CHECK_SET(X) X.Check()
#else
#define CHECK_SET(X) (void)X
#endif

Persistent<Function> AdBlockClientWrap::constructor;

AdBlockClientWrap::AdBlockClientWrap() {
}

AdBlockClientWrap::~AdBlockClientWrap() {
}

Local<Object> ToLocalObject(Isolate* isolate, const FilterList& filter_list) {
  Local<Object> list = Object::New(isolate);
  CHECK_SET(list->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "uuid", NewStringType::kNormal).ToLocalChecked(),
    String::NewFromUtf8(isolate, filter_list.uuid.c_str(), NewStringType::kNormal).ToLocalChecked()));
  CHECK_SET(list->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "listURL", NewStringType::kNormal).ToLocalChecked(),
    String::NewFromUtf8(isolate, filter_list.url.c_str(), NewStringType::kNormal).ToLocalChecked()));
  CHECK_SET(list->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "title", NewStringType::kNormal).ToLocalChecked(),
    String::NewFromUtf8(isolate, filter_list.title.c_str(), NewStringType::kNormal).ToLocalChecked()));
  CHECK_SET(list->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "supportURL", NewStringType::kNormal).ToLocalChecked(),
    String::NewFromUtf8(isolate, filter_list.support_url.c_str(), NewStringType::kNormal).ToLocalChecked()));
  CHECK_SET(list->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "componentID", NewStringType::kNormal).ToLocalChecked(),
    String::NewFromUtf8(isolate, filter_list.component_id.c_str(), NewStringType::kNormal).ToLocalChecked()));
  CHECK_SET(list->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "base64PublicKey", NewStringType::kNormal).ToLocalChecked(),
    String::NewFromUtf8(isolate, filter_list.base64_public_key.c_str(), NewStringType::kNormal).ToLocalChecked()));
  Local<Array> langs = Array::New(isolate);
  int j = 0;
  std::for_each(filter_list.langs.begin(), filter_list.langs.end(),
    [&isolate, &langs, &j](const std::string &lang) {
    CHECK_SET(langs->Set(isolate->GetCurrentContext(), j++,
      String::NewFromUtf8(isolate, lang.c_str(), NewStringType::kNormal).ToLocalChecked()));
  });
  if (filter_list.langs.size() > 0) {
    CHECK_SET(list->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "langs", NewStringType::kNormal).ToLocalChecked(), langs));
  }
  return list;
}

Local<Array> ToLocalObject(Isolate* isolate,
    const std::vector<FilterList> &list) {
  Local<Array> lists = Array::New(isolate);
  int j = 0;
  std::for_each(list.begin(), list.end(),
    [&isolate, &lists, &j](const FilterList &filter_list) {
    CHECK_SET(lists->Set(isolate->GetCurrentContext(), j++, ToLocalObject(isolate, filter_list)));
  });
  return lists;
}

void GenerateManifestFile(const std::string &name,
                          const std::string &base64_public_key,
                          const std::string &filename) {
  std::ofstream outFile(filename);
  if (outFile.is_open()) {
    outFile << "{" << std::endl;
    outFile << "  \"description\": \"Brave Ad Block Updater extension\","
      << std::endl;
    outFile << "  \"key\": \"" << base64_public_key << "\"," << std::endl;
    outFile << "  \"manifest_version\": 2," << std::endl;
    outFile << "  \"name\": \"Brave Ad Block Updater (" << name << ")\","
      << std::endl;
    outFile << "  \"version\": \"0.0.0\"" << std::endl;
    outFile << "}" << std::endl;
  }
}

void AdBlockClientWrap::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "AdBlockClient", NewStringType::kNormal).ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "clear", AdBlockClientWrap::Clear);
  NODE_SET_PROTOTYPE_METHOD(tpl, "parse", AdBlockClientWrap::Parse);
  NODE_SET_PROTOTYPE_METHOD(tpl, "matches", AdBlockClientWrap::Matches);
  NODE_SET_PROTOTYPE_METHOD(tpl, "findMatchingFilters",
      AdBlockClientWrap::FindMatchingFilters);
  NODE_SET_PROTOTYPE_METHOD(tpl, "serialize", AdBlockClientWrap::Serialize);
  NODE_SET_PROTOTYPE_METHOD(tpl, "deserialize",
    AdBlockClientWrap::Deserialize);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getParsingStats",
    AdBlockClientWrap::GetParsingStats);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getFilters",
    AdBlockClientWrap::GetFilters);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getFingerprint",
    AdBlockClientWrap::GetFingerprint);
  NODE_SET_PROTOTYPE_METHOD(tpl, "getMatchingStats",
    AdBlockClientWrap::GetMatchingStats);
  NODE_SET_PROTOTYPE_METHOD(tpl, "enableBadFingerprintDetection",
    AdBlockClientWrap::EnableBadFingerprintDetection);
  NODE_SET_PROTOTYPE_METHOD(tpl, "generateBadFingerprintsHeader",
    AdBlockClientWrap::GenerateBadFingerprintsHeader);
  NODE_SET_PROTOTYPE_METHOD(tpl, "generateDefaultManifestFile",
    AdBlockClientWrap::GenerateDefaultManifestFile);
  NODE_SET_PROTOTYPE_METHOD(tpl, "generateRegionalManifestFiles",
    AdBlockClientWrap::GenerateRegionalManifestFiles);
  NODE_SET_PROTOTYPE_METHOD(tpl, "cleanup", AdBlockClientWrap::Cleanup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addTag", AdBlockClientWrap::AddTag);
  NODE_SET_PROTOTYPE_METHOD(tpl, "removeTag", AdBlockClientWrap::RemoveTag);

  // filter options
  Local<Object> filterOptions = Object::New(isolate);
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "noFilterOption", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FONoFilterOption)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "script", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOScript)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "image", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOImage)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "stylesheet", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOStylesheet)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "object", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOObject)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "xmlHttpRequest", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOXmlHttpRequest)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "objectSubrequest", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOObjectSubrequest)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "subdocument", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOSubdocument)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "document", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FODocument)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "other", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOOther)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "xbl", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOXBL)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "collapse", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOCollapse)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "doNotTrack", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FODoNotTrack)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "elemHide", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOElemHide)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "thirdParty", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOThirdParty)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "notThirdParty", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FONotThirdParty)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "ping", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOPing)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "popup", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOPopup)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "redirect", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FORedirect)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "csp", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOCSP)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "font", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOFont)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "media", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOMedia)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "webrtc", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOWebRTC)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "genericblock", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOGenericBlock)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "generichide", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOGenericHide)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "empty", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOEmpty)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "websocket", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOWebsocket)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "important", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOImportant)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "explicitcancel", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOExplicitCancel)));
  CHECK_SET(filterOptions->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "unknown", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, FOUnknown)));

  // Adblock lists
  Local<Object> lists = Object::New(isolate);
  CHECK_SET(lists->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "default", NewStringType::kNormal).ToLocalChecked(),
    ToLocalObject(isolate, default_lists)));
  CHECK_SET(lists->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "malware", NewStringType::kNormal).ToLocalChecked(),
    ToLocalObject(isolate, malware_lists)));
  CHECK_SET(lists->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "regions", NewStringType::kNormal).ToLocalChecked(),
    ToLocalObject(isolate, region_lists)));
  constructor.Reset(isolate,
      tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
  CHECK_SET(exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "AdBlockClient", NewStringType::kNormal).ToLocalChecked(),
      tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked()));
  CHECK_SET(exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "FilterOptions", NewStringType::kNormal).ToLocalChecked(), filterOptions));
  CHECK_SET(exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "adBlockLists", NewStringType::kNormal).ToLocalChecked(), lists));
  CHECK_SET(exports->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "adBlockDataFileVersion", NewStringType::kNormal).ToLocalChecked(),
               Int32::New(isolate, DATA_FILE_VERSION)));
}

void AdBlockClientWrap::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new AdBlockClient(...)`
    AdBlockClientWrap* obj = new AdBlockClientWrap();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `AdBlockClient(...)`,
    // turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(
        cons->NewInstance(isolate->GetCurrentContext(), argc, argv).ToLocalChecked());
  }
}

void AdBlockClientWrap::Clear(const FunctionCallbackInfo<Value>& args) {
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  obj->clear();
}

void AdBlockClientWrap::Parse(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
#if V8_MAJOR_VERSION >= 7
  bool preserveRules(args[1]->BooleanValue(isolate));
#else
  bool preserveRules(args[1]->BooleanValue());
#endif
  const char * buffer = *str;

  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  obj->parse(buffer, preserveRules);
}

void AdBlockClientWrap::Matches(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
  const char * buffer = *str;
  int32_t filterOption =
    static_cast<FilterOption>(args[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0));
  String::Utf8Value currentPageDomain(isolate, args[2]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
  const char * currentPageDomainBuffer = *currentPageDomain;

  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  bool matches = obj->matches(buffer,
    static_cast<FilterOption>(filterOption),
    currentPageDomainBuffer);

  args.GetReturnValue().Set(Boolean::New(isolate, matches));
}

void AdBlockClientWrap::FindMatchingFilters(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
  const char * buffer = *str;
  int32_t filterOption =
      static_cast<FilterOption>(args[1]->Int32Value(isolate->GetCurrentContext()).FromMaybe(0));
  String::Utf8Value currentPageDomain(isolate, args[2]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
  const char * currentPageDomainBuffer = *currentPageDomain;

  Filter *matchingFilter;
  Filter *matchingExceptionFilter;
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  bool matches = obj->findMatchingFilters(buffer,
    static_cast<FilterOption>(filterOption),
    currentPageDomainBuffer, &matchingFilter, &matchingExceptionFilter);

  Local<Object> foundData = Object::New(isolate);
  CHECK_SET(foundData->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "matches", NewStringType::kNormal).ToLocalChecked(),
    Boolean::New(isolate, matches)));
  if (matchingFilter) {
    CHECK_SET(foundData->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "matchingFilter", NewStringType::kNormal).ToLocalChecked(),
      String::NewFromUtf8(isolate, matchingFilter->data, NewStringType::kNormal).ToLocalChecked()));
    if (matchingFilter->ruleDefinition != nullptr) {
      CHECK_SET(foundData->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "matchingOrigRule", NewStringType::kNormal).ToLocalChecked(),
        String::NewFromUtf8(isolate, matchingFilter->ruleDefinition, NewStringType::kNormal).ToLocalChecked()));
    }
  }
  if (matchingExceptionFilter) {
    CHECK_SET(foundData->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "matchingExceptionFilter", NewStringType::kNormal).ToLocalChecked(),
      String::NewFromUtf8(isolate, matchingExceptionFilter->data, NewStringType::kNormal).ToLocalChecked()));
    if (matchingExceptionFilter->ruleDefinition != nullptr) {
      CHECK_SET(foundData->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "matchingExceptionOrigRule", NewStringType::kNormal).ToLocalChecked(),
        String::NewFromUtf8(isolate, matchingExceptionFilter->ruleDefinition, NewStringType::kNormal).ToLocalChecked()));
    }
  }
  args.GetReturnValue().Set(foundData);
}

void AdBlockClientWrap::Serialize(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());

  int totalSize = 0;
  // Serialize data
  char* data = obj->serialize(&totalSize);
  if (nullptr == data) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Could not serialize", NewStringType::kNormal).ToLocalChecked()));
    return;
  }

  MaybeLocal<Object> buffer = node::Buffer::New(isolate, totalSize);
  Local<Object> localBuffer;
  if (!buffer.ToLocal(&localBuffer)) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Could not convert MaybeLocal to Local", NewStringType::kNormal).ToLocalChecked()));
    return;
  }
  memcpy(node::Buffer::Data(localBuffer), data, totalSize);
  delete[] data;
  args.GetReturnValue().Set(localBuffer);
}
void AdBlockClientWrap::Deserialize(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());

  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Wrong number of arguments", NewStringType::kNormal).ToLocalChecked()));
    return;
  }

  if (!args[0]->IsArrayBufferView()) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate,
        "Provided string is not valid, serialized DAT data", NewStringType::kNormal).ToLocalChecked()));
    return;
  }

  unsigned char *buf = (unsigned char *)node::Buffer::Data(args[0]);
  size_t length = node::Buffer::Length(args[0]);
  const char *oldDeserializedData = obj->getDeserializedBuffer();
  if (nullptr != oldDeserializedData) {
    delete []oldDeserializedData;
  }
  char *deserializedData = new char[length];
  memcpy(deserializedData, buf, length);
  args.GetReturnValue().Set(Boolean::New(isolate,
    obj->deserialize(deserializedData)));
}

void AdBlockClientWrap::AddTag(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
  const char * buffer = *str;

  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  obj->addTag(buffer);
}

void AdBlockClientWrap::RemoveTag(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
  const char * buffer = *str;

  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  obj->removeTag(buffer);
}

void AdBlockClientWrap::GetParsingStats(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  Local<Object> stats = Object::New(isolate);
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numFilters", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numFilters)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numCosmeticFilters", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numCosmeticFilters)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numExceptionFilters", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numExceptionFilters)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numNoFingerprintFilters", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numNoFingerprintFilters)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numNoFingerprintDomainOnlyFilters", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numNoFingerprintDomainOnlyFilters)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate,
        "numNoFingerprintAntiDomainOnlyFilters", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numNoFingerprintAntiDomainOnlyFilters)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numNoFingerprintExceptionFilters", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numNoFingerprintExceptionFilters)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate,
        "numNoFingerprintDomainOnlyExceptionFilters", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numNoFingerprintDomainOnlyExceptionFilters)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate,
        "numNoFingerprintAntiDomainOnlyExceptionFilters", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numNoFingerprintAntiDomainOnlyExceptionFilters)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numHostAnchoredFilters", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numHostAnchoredFilters)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numHostAnchoredExceptionFilters", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numHostAnchoredExceptionFilters)));
  args.GetReturnValue().Set(stats);
}

void AdBlockClientWrap::GetFilters(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());

  String::Utf8Value str(isolate, args[0]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
  const char * filterType = *str;

  Local<Array> result_list = Array::New(isolate);
  Filter *filter;
  int numFilters = 0;

  if (!strcmp(filterType, "filters")) {
    filter = obj->filters;
    numFilters = obj->numFilters;
  } else if (!strcmp(filterType, "cosmeticFilters")) {
    filter = obj->cosmeticFilters;
    numFilters = obj->numCosmeticFilters;
  } else if (!strcmp(filterType, "htmlFilters")) {
    filter = obj->htmlFilters;
    numFilters = obj->numHtmlFilters;
  } else if (!strcmp(filterType, "exceptionFilters")) {
    filter = obj->exceptionFilters;
    numFilters = obj->numExceptionFilters;
  } else if (!strcmp(filterType, "noFingerprintFilters")) {
    filter = obj->noFingerprintFilters;
    numFilters = obj->numNoFingerprintFilters;
  } else if (!strcmp(filterType, "noFingerprintExceptionFilters")) {
    filter = obj->noFingerprintExceptionFilters;
    numFilters = obj->numNoFingerprintExceptionFilters;
  } else if (!strcmp(filterType, "noFingerprintDomainOnlyFilters")) {
    filter = obj->noFingerprintDomainOnlyFilters;
    numFilters = obj->numNoFingerprintDomainOnlyFilters;
  } else if (!strcmp(filterType, "noFingerprintAntiDomainOnlyFilters")) {
    filter = obj->noFingerprintAntiDomainOnlyFilters;
    numFilters = obj->numNoFingerprintAntiDomainOnlyFilters;
  } else if (!strcmp(filterType, "noFingerprintDomainOnlyExceptionFilters")) {
    filter = obj->noFingerprintDomainOnlyExceptionFilters;
    numFilters = obj->numNoFingerprintDomainOnlyExceptionFilters;
  } else if (!strcmp(filterType,
        "noFingerprintAntiDomainOnlyExceptionFilters")) {
    filter = obj->noFingerprintAntiDomainOnlyExceptionFilters;
    numFilters = obj->numNoFingerprintAntiDomainOnlyExceptionFilters;
  }

  for (int i = 0; i < numFilters; i++) {
    Local<Object> result = Object::New(isolate);
    if (filter->data && filter->dataLen) {
      if (filter->dataLen == -1) {
        filter->dataLen = static_cast<int>(strlen(filter->data));
      }
      char * data = new char[filter->dataLen + 1];
      data[filter->dataLen] = '\0';
      memcpy(data, filter->data, filter->dataLen);
      CHECK_SET(result->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "data", NewStringType::kNormal).ToLocalChecked(),
        String::NewFromUtf8(isolate, data, NewStringType::kNormal).ToLocalChecked()));
      delete[] data;
    }
    if (filter->host && filter->hostLen) {
      if (filter->hostLen == -1) {
        filter->hostLen = static_cast<int>(strlen(filter->host));
      }
      char * host = new char[filter->hostLen + 1];
      host[filter->hostLen] = '\0';
      memcpy(host, filter->host, filter->hostLen);
      CHECK_SET(result->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "host", NewStringType::kNormal).ToLocalChecked(),
        String::NewFromUtf8(isolate, host, NewStringType::kNormal).ToLocalChecked()));
      delete[] host;
    }

    Local<Array> domain_list = Array::New(isolate);
    Local<Array> anti_domain_list = Array::New(isolate);
    if (filter->domainList) {
      char * filter_domain_list = filter->domainList;
      size_t domain_list_len = strlen(filter_domain_list);
      // Setup a buffer the max size of a domain, we'll use it for each domain
      // which is less or the same length.
      char * dest_buffer = new char[domain_list_len + 1];
      memset(dest_buffer, 0, domain_list_len + 1);
      int start_offset = 0;
      int len = 0;
      int domain_list_count = 0;
      int anti_domain_list_count = 0;
      const char *p = filter_domain_list;
      while (true) {
        if (*p == '|' || *p == '\0') {
          const char *domain = filter_domain_list + start_offset;
          if (len > 0 && *domain != '~') {
            memcpy(dest_buffer, domain, len);
            CHECK_SET(domain_list->Set(isolate->GetCurrentContext(), domain_list_count++,
              String::NewFromUtf8(isolate, dest_buffer, NewStringType::kNormal).ToLocalChecked()));
          } else if (len > 0 && *domain == '~') {
            memcpy(dest_buffer, domain + 1, len - 1);
            CHECK_SET(anti_domain_list->Set(isolate->GetCurrentContext(), anti_domain_list_count++,
              String::NewFromUtf8(isolate, dest_buffer, NewStringType::kNormal).ToLocalChecked()));
          }

          start_offset += len + 1;
          len = -1;
          memset(dest_buffer, 0, domain_list_len + 1);
        }
        if (*p == '\0') {
          break;
        }
        p++;
        len++;
      }
      delete[] dest_buffer;
    }
    CHECK_SET(result->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "isDomainOnlyFilter", NewStringType::kNormal).ToLocalChecked(),
        Boolean::New(isolate, filter->isDomainOnlyFilter() &&
          ((~filter->filterType) & FTException))));
    CHECK_SET(result->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "isAntiDomainOnlyFilter", NewStringType::kNormal).ToLocalChecked(),
        Boolean::New(isolate, filter->isAntiDomainOnlyFilter() &&
          ((~filter->filterType) & FTException))));
    CHECK_SET(result->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "isDomainOnlyExceptionFilter", NewStringType::kNormal).ToLocalChecked(),
        Boolean::New(isolate, filter->isDomainOnlyFilter() &&
          (filter->filterType & FTException))));
    CHECK_SET(result->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate,
          "isAntiDomainOnlyExceptionFilter", NewStringType::kNormal).ToLocalChecked(),
        Boolean::New(isolate, filter->isAntiDomainOnlyFilter() &&
          (filter->filterType & FTException))));
    CHECK_SET(result->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "domainList", NewStringType::kNormal).ToLocalChecked(), domain_list));
    CHECK_SET(result->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate,
          "antiDomainList", NewStringType::kNormal).ToLocalChecked(), anti_domain_list));
    CHECK_SET(result->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "tag", NewStringType::kNormal).ToLocalChecked(),
        String::NewFromUtf8(isolate,
          std::string(filter->tagLen > 0 ? filter->tag : "",
            filter->tagLen).c_str(), NewStringType::kNormal).ToLocalChecked()));
    CHECK_SET(result_list->Set(isolate->GetCurrentContext(), i, result));
    filter++;
  }
  args.GetReturnValue().Set(result_list);
}

void AdBlockClientWrap::GetFingerprint(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  String::Utf8Value str(isolate, args[0]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
  const char * inputBuffer = *str;

  char * fingerprintBuffer = new char[AdBlockClient::kFingerprintSize + 1];
  if (obj->getFingerprint(fingerprintBuffer, inputBuffer)) {
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, fingerprintBuffer, NewStringType::kNormal).ToLocalChecked());
  }
  delete[] fingerprintBuffer;
}

void AdBlockClientWrap::GetMatchingStats(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  Local<Object> stats = Object::New(isolate);
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numFalsePositives", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numFalsePositives)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numExceptionFalsePositives", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numExceptionFalsePositives)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numBloomFilterSaves", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numBloomFilterSaves)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numExceptionBloomFilterSaves", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numExceptionBloomFilterSaves)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numHashSetSaves", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numHashSetSaves)));
  CHECK_SET(stats->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "numExceptionHashSetSaves", NewStringType::kNormal).ToLocalChecked(),
    Int32::New(isolate, obj->numExceptionHashSetSaves)));
  args.GetReturnValue().Set(stats);
}

void AdBlockClientWrap::EnableBadFingerprintDetection(
    const FunctionCallbackInfo<Value>& args) {
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  obj->enableBadFingerprintDetection();
}

void AdBlockClientWrap::GenerateBadFingerprintsHeader(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
  const char * filename = *str;
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  obj->badFingerprintsHashSet->generateHeader(filename);
}

void AdBlockClientWrap::GenerateDefaultManifestFile(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
  const char * dir = *str;
  std::string filename = dir + std::string("/default-manifest.json");
  GenerateManifestFile("Default", kAdBlockDefaultBase64PublicKey, filename);
}

void AdBlockClientWrap::GenerateRegionalManifestFiles(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString(isolate->GetCurrentContext())
      .FromMaybe(Local<String>()));
  const char * dir = *str;
  for (auto& entry : region_lists) {
    const std::string filename = (
      dir +
      std::string("/") +
      entry.uuid +
      std::string("-manifest.json"));
    GenerateManifestFile(entry.title, entry.base64_public_key, filename);
  }
}

void AdBlockClientWrap::Cleanup(const FunctionCallbackInfo<Value>& args) {
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  const char *deserializedData = obj->getDeserializedBuffer();
  if (nullptr != deserializedData) {
    delete []deserializedData;
    deserializedData = nullptr;
  }
  delete obj;
}

}  // namespace ad_block_client_wrap

