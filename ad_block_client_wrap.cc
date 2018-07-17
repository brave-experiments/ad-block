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

Persistent<Function> AdBlockClientWrap::constructor;

AdBlockClientWrap::AdBlockClientWrap() {
}

AdBlockClientWrap::~AdBlockClientWrap() {
}

Local<Object> ToLocalObject(Isolate* isolate, const FilterList& filter_list) {
  Local<Object> list = Object::New(isolate);
  list->Set(String::NewFromUtf8(isolate, "uuid"),
    String::NewFromUtf8(isolate, filter_list.uuid.c_str()));
  list->Set(String::NewFromUtf8(isolate, "listURL"),
    String::NewFromUtf8(isolate, filter_list.url.c_str()));
  list->Set(String::NewFromUtf8(isolate, "title"),
    String::NewFromUtf8(isolate, filter_list.title.c_str()));
  list->Set(String::NewFromUtf8(isolate, "supportURL"),
    String::NewFromUtf8(isolate, filter_list.support_url.c_str()));
  list->Set(String::NewFromUtf8(isolate, "componentID"),
    String::NewFromUtf8(isolate, filter_list.component_id.c_str()));
  list->Set(String::NewFromUtf8(isolate, "base64PublicKey"),
    String::NewFromUtf8(isolate, filter_list.base64_public_key.c_str()));

  Local<Array> langs = Array::New(isolate);
  int j = 0;
  std::for_each(filter_list.langs.begin(), filter_list.langs.end(),
    [&isolate, &langs, &j](const std::string &lang) {
    langs->Set(j++,
      String::NewFromUtf8(isolate, lang.c_str()));
  });
  if (filter_list.langs.size() > 0) {
    list->Set(String::NewFromUtf8(isolate, "langs"), langs);
  }
  return list;
}

Local<Array> ToLocalObject(Isolate* isolate,
    const std::vector<FilterList> &list) {
  Local<Array> lists = Array::New(isolate);
  int j = 0;
  std::for_each(list.begin(), list.end(),
    [&isolate, &lists, &j](const FilterList &filter_list) {
    lists->Set(j++, ToLocalObject(isolate, filter_list));
  });
  return lists;
}

void GenerateManifestFile(const std::string &name,
                          const std::string &base64_public_key,
                          const std::string &filename) {
  std::ofstream outFile(filename);
  if (outFile.is_open()) {
    outFile << "{" << std::endl;
    outFile << "  \"description\": \"Brave Ad Block Updater extension\"," << std::endl;
    outFile << "  \"key\": \"" << base64_public_key << "\"," << std::endl;
    outFile << "  \"manifest_version\": 2," << std::endl;
    outFile << "  \"name\": \"Brave Ad Block Updater (" << name << ")\"," << std::endl;
    outFile << "  \"version\": \"0.0.0\"" << std::endl;
    outFile << "}" << std::endl;
  }
}

void AdBlockClientWrap::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "AdBlockClient"));
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

  // filter options
  Local<Object> filterOptions = Object::New(isolate);
  filterOptions->Set(String::NewFromUtf8(isolate, "noFilterOption"),
    Int32::New(isolate, 0));
  filterOptions->Set(String::NewFromUtf8(isolate, "script"),
    Int32::New(isolate, 01));
  filterOptions->Set(String::NewFromUtf8(isolate, "image"),
    Int32::New(isolate, 02));
  filterOptions->Set(String::NewFromUtf8(isolate, "stylesheet"),
    Int32::New(isolate, 04));
  filterOptions->Set(String::NewFromUtf8(isolate, "object"),
    Int32::New(isolate, 010));
  filterOptions->Set(String::NewFromUtf8(isolate, "xmlHttpRequest"),
    Int32::New(isolate, 020));
  filterOptions->Set(String::NewFromUtf8(isolate, "objectSubrequest"),
    Int32::New(isolate, 040));
  filterOptions->Set(String::NewFromUtf8(isolate, "subdocument"),
    Int32::New(isolate, 0100));
  filterOptions->Set(String::NewFromUtf8(isolate, "document"),
    Int32::New(isolate, 0200));
  filterOptions->Set(String::NewFromUtf8(isolate, "other"),
    Int32::New(isolate, 0400));
  filterOptions->Set(String::NewFromUtf8(isolate, "xbl"),
    Int32::New(isolate, 01000));
  filterOptions->Set(String::NewFromUtf8(isolate, "collapse"),
    Int32::New(isolate, 02000));
  filterOptions->Set(String::NewFromUtf8(isolate, "doNotTrack"),
    Int32::New(isolate, 04000));
  filterOptions->Set(String::NewFromUtf8(isolate, "elemHide"),
    Int32::New(isolate, 010000));
  filterOptions->Set(String::NewFromUtf8(isolate, "thirdParty"),
    Int32::New(isolate, 020000));
  filterOptions->Set(String::NewFromUtf8(isolate, "notThirdParty"),
    Int32::New(isolate, 040000));
  filterOptions->Set(String::NewFromUtf8(isolate, "ping"),
    Int32::New(isolate, 0100000));
  filterOptions->Set(String::NewFromUtf8(isolate, "popup"),
    Int32::New(isolate, 0200000));
  filterOptions->Set(String::NewFromUtf8(isolate, "font"),
    Int32::New(isolate, 02000000));
  filterOptions->Set(String::NewFromUtf8(isolate, "media"),
    Int32::New(isolate, 04000000));
  filterOptions->Set(String::NewFromUtf8(isolate, "webrtc"),
    Int32::New(isolate, 010000000));
  filterOptions->Set(String::NewFromUtf8(isolate, "empty"),
    Int32::New(isolate, 0100000000));
  filterOptions->Set(String::NewFromUtf8(isolate, "websocket"),
    Int32::New(isolate, 0200000000));

  // Adblock lists
  Local<Object> lists = Object::New(isolate);
  lists->Set(String::NewFromUtf8(isolate, "default"),
    ToLocalObject(isolate, default_lists));
  lists->Set(String::NewFromUtf8(isolate, "malware"),
    ToLocalObject(isolate, malware_lists));
  lists->Set(String::NewFromUtf8(isolate, "regions"),
    ToLocalObject(isolate, region_lists));

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "AdBlockClient"),
               tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "FilterOptions"), filterOptions);
  exports->Set(String::NewFromUtf8(isolate, "adBlockLists"), lists);
  exports->Set(String::NewFromUtf8(isolate, "adBlockDataFileVersion"),
               Int32::New(isolate, DATA_FILE_VERSION));
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
        cons->NewInstance(isolate->GetCurrentContext(), argc, argv)
            .ToLocalChecked());
  }
}

void AdBlockClientWrap::Clear(const FunctionCallbackInfo<Value>& args) {
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  obj->clear();
}

void AdBlockClientWrap::Parse(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString());
  bool preserveRules(args[1]->BooleanValue());
  const char * buffer = *str;

  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  obj->parse(buffer, preserveRules);
}

void AdBlockClientWrap::Matches(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString());
  const char * buffer = *str;
  int32_t filterOption = static_cast<FilterOption>(args[1]->Int32Value());
  String::Utf8Value currentPageDomain(isolate, args[2]->ToString());
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
  String::Utf8Value str(isolate, args[0]->ToString());
  const char * buffer = *str;
  int32_t filterOption = static_cast<FilterOption>(args[1]->Int32Value());
  String::Utf8Value currentPageDomain(isolate, args[2]->ToString());
  const char * currentPageDomainBuffer = *currentPageDomain;

  Filter *matchingFilter;
  Filter *matchingExceptionFilter;
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  bool matches = obj->findMatchingFilters(buffer,
    static_cast<FilterOption>(filterOption),
    currentPageDomainBuffer, &matchingFilter, &matchingExceptionFilter);

  Local<Object> foundData = Object::New(isolate);
  foundData->Set(String::NewFromUtf8(isolate, "matches"),
    Boolean::New(isolate, matches));
  if (matchingFilter) {
    foundData->Set(String::NewFromUtf8(isolate, "matchingFilter"),
      String::NewFromUtf8(isolate, matchingFilter->data));
    if (matchingFilter->ruleDefinition != nullptr) {
      foundData->Set(String::NewFromUtf8(isolate, "matchingOrigRule"),
        String::NewFromUtf8(isolate, matchingFilter->ruleDefinition));
    }
  }
  if (matchingExceptionFilter) {
    foundData->Set(String::NewFromUtf8(isolate, "matchingExceptionFilter"),
      String::NewFromUtf8(isolate, matchingExceptionFilter->data));
    if (matchingExceptionFilter->ruleDefinition != nullptr) {
      foundData->Set(String::NewFromUtf8(isolate, "matchingExceptionOrigRule"),
        String::NewFromUtf8(isolate, matchingExceptionFilter->ruleDefinition));
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
      String::NewFromUtf8(isolate, "Could not serialize")));
    return;
  }

  MaybeLocal<Object> buffer = node::Buffer::New(isolate, totalSize);
  Local<Object> localBuffer;
  if (!buffer.ToLocal(&localBuffer)) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Could not convert MaybeLocal to Local")));
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
      String::NewFromUtf8(isolate, "Wrong number of arguments")));
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

void AdBlockClientWrap::GetParsingStats(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  Local<Object> stats = Object::New(isolate);
  stats->Set(String::NewFromUtf8(isolate, "numFilters"),
    Int32::New(isolate, obj->numFilters));
  stats->Set(String::NewFromUtf8(isolate, "numCosmeticFilters"),
    Int32::New(isolate, obj->numCosmeticFilters));
  stats->Set(String::NewFromUtf8(isolate, "numExceptionFilters"),
    Int32::New(isolate, obj->numExceptionFilters));
  stats->Set(String::NewFromUtf8(isolate, "numNoFingerprintFilters"),
    Int32::New(isolate, obj->numNoFingerprintFilters));
  stats->Set(String::NewFromUtf8(isolate, "numNoFingerprintDomainOnlyFilters"),
    Int32::New(isolate, obj->numNoFingerprintDomainOnlyFilters));
  stats->Set(String::NewFromUtf8(isolate,
        "numNoFingerprintAntiDomainOnlyFilters"),
    Int32::New(isolate, obj->numNoFingerprintAntiDomainOnlyFilters));
  stats->Set(String::NewFromUtf8(isolate, "numNoFingerprintExceptionFilters"),
    Int32::New(isolate, obj->numNoFingerprintExceptionFilters));
  stats->Set(String::NewFromUtf8(isolate,
        "numNoFingerprintDomainOnlyExceptionFilters"),
    Int32::New(isolate, obj->numNoFingerprintDomainOnlyExceptionFilters));
  stats->Set(String::NewFromUtf8(isolate,
        "numNoFingerprintAntiDomainOnlyExceptionFilters"),
    Int32::New(isolate, obj->numNoFingerprintAntiDomainOnlyExceptionFilters));
  stats->Set(String::NewFromUtf8(isolate, "numHostAnchoredFilters"),
    Int32::New(isolate, obj->numHostAnchoredFilters));
  stats->Set(String::NewFromUtf8(isolate, "numHostAnchoredExceptionFilters"),
    Int32::New(isolate, obj->numHostAnchoredExceptionFilters));
  args.GetReturnValue().Set(stats);
}

void AdBlockClientWrap::GetFilters(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());

  String::Utf8Value str(isolate, args[0]->ToString());
  const char * filterType = *str;

  Local<v8::Array> result_list = v8::Array::New(isolate);
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
      result->Set(String::NewFromUtf8(isolate, "data"),
        String::NewFromUtf8(isolate, data));
      delete[] data;
    }
    if (filter->host && filter->hostLen) {
      if (filter->hostLen == -1) {
        filter->hostLen = static_cast<int>(strlen(filter->host));
      }
      char * host = new char[filter->hostLen + 1];
      host[filter->hostLen] = '\0';
      memcpy(host, filter->host, filter->hostLen);
      result->Set(String::NewFromUtf8(isolate, "host"),
        String::NewFromUtf8(isolate, host));
      delete[] host;
    }

    Local<v8::Array> domain_list = v8::Array::New(isolate);
    Local<v8::Array> anti_domain_list = v8::Array::New(isolate);
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
            domain_list->Set(domain_list_count++,
              String::NewFromUtf8(isolate, dest_buffer));
          } else if (len > 0 && *domain == '~') {
            memcpy(dest_buffer, domain + 1, len - 1);
            anti_domain_list->Set(anti_domain_list_count++,
              String::NewFromUtf8(isolate, dest_buffer));
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
    result->Set(String::NewFromUtf8(isolate, "isDomainOnlyFilter"),
        Boolean::New(isolate, filter->isDomainOnlyFilter() &&
          ((~filter->filterType) & FTException)));
    result->Set(String::NewFromUtf8(isolate, "isAntiDomainOnlyFilter"),
        Boolean::New(isolate, filter->isAntiDomainOnlyFilter() &&
          ((~filter->filterType) & FTException)));
    result->Set(String::NewFromUtf8(isolate, "isDomainOnlyExceptionFilter"),
        Boolean::New(isolate, filter->isDomainOnlyFilter() &&
          (filter->filterType & FTException)));
    result->Set(String::NewFromUtf8(isolate,
          "isAntiDomainOnlyExceptionFilter"),
        Boolean::New(isolate, filter->isAntiDomainOnlyFilter() &&
          (filter->filterType & FTException)));

    result->Set(String::NewFromUtf8(isolate, "domainList"), domain_list);
    result->Set(String::NewFromUtf8(isolate,
          "antiDomainList"), anti_domain_list);

    result_list->Set(i, result);
    filter++;
  }
  args.GetReturnValue().Set(result_list);
}

void AdBlockClientWrap::GetFingerprint(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  String::Utf8Value str(isolate, args[0]->ToString());
  const char * inputBuffer = *str;

  char * fingerprintBuffer = new char[AdBlockClient::kFingerprintSize + 1];
  if (obj->getFingerprint(fingerprintBuffer, inputBuffer)) {
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, fingerprintBuffer));
  }
  delete[] fingerprintBuffer;
}

void AdBlockClientWrap::GetMatchingStats(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  Local<Object> stats = Object::New(isolate);
  stats->Set(String::NewFromUtf8(isolate, "numFalsePositives"),
    Int32::New(isolate, obj->numFalsePositives));
  stats->Set(String::NewFromUtf8(isolate, "numExceptionFalsePositives"),
    Int32::New(isolate, obj->numExceptionFalsePositives));
  stats->Set(String::NewFromUtf8(isolate, "numBloomFilterSaves"),
    Int32::New(isolate, obj->numBloomFilterSaves));
  stats->Set(String::NewFromUtf8(isolate, "numExceptionBloomFilterSaves"),
    Int32::New(isolate, obj->numExceptionBloomFilterSaves));
  stats->Set(String::NewFromUtf8(isolate, "numHashSetSaves"),
    Int32::New(isolate, obj->numHashSetSaves));
  stats->Set(String::NewFromUtf8(isolate, "numExceptionHashSetSaves"),
    Int32::New(isolate, obj->numExceptionHashSetSaves));
  args.GetReturnValue().Set(stats);
}

void AdBlockClientWrap::EnableBadFingerprintDetection(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  obj->enableBadFingerprintDetection();
}

void AdBlockClientWrap::GenerateBadFingerprintsHeader(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString());
  const char * filename = *str;
  AdBlockClientWrap* obj =
    ObjectWrap::Unwrap<AdBlockClientWrap>(args.Holder());
  obj->badFingerprintsHashSet->generateHeader(filename);
}

void AdBlockClientWrap::GenerateDefaultManifestFile(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString());
  const char * dir = *str;
  std::string filename = dir + std::string("/default-manifest.json");
  GenerateManifestFile("Default", kAdBlockDefaultBase64PublicKey, filename);
}

void AdBlockClientWrap::GenerateRegionalManifestFiles(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  String::Utf8Value str(isolate, args[0]->ToString());
  const char * dir = *str;
  for (auto& entry : region_lists) {
    std::string filename = dir + std::string("/") + entry.uuid + std::string("-manifest.json");
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

