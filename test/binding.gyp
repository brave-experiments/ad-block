{
  "targets": [{
    "target_name": "ad-block-test",
    "type": "executable",
    "sources": [
      "../test/test_main.cc",
      "../test/parser_test.cc",
      "../test/options_test.cc",
      "../test/rule_types_test.cc",
      "../test/cosmetic_filter_test.cc",
      "../test/protocol_test.cc",
      "../test/orig_filters_test.cc",
      "../test/serialization_test.cc",
      "../test/util.cc",
      "../protocol.cc",
      "../protocol.h",
      "../ad_block_client.cc",
      "../ad_block_client.h",
      "../context_domain.cc",
      "../context_domain.h",
      "../cosmetic_filter.cc",
      "../cosmetic_filter.h",
      "../filter.cc",
      "../filter.h",
      "../filter_list.cc",
      "../filter_list.h",
      "../no_fingerprint_domain.cc",
      "../no_fingerprint_domain.h",
      "../node_modules/bloom-filter-cpp/BloomFilter.cpp",
      "../node_modules/bloom-filter-cpp/BloomFilter.h",
      "../node_modules/bloom-filter-cpp/hashFn.cpp",
      "../node_modules/bloom-filter-cpp/hashFn.h",
      "../node_modules/hashset-cpp/hash_set.cc",
      "../node_modules/hashset-cpp/hash_set.h"
    ],
    "include_dirs": [
      "..",
      '../node_modules/bloom-filter-cpp',
      '../node_modules/hashset-cpp',
      '../node_modules/cppunitlite',
      '../node_modules/nan'
      "..",
    ],
    "dependencies": [
      "../node_modules/cppunitlite/binding.gyp:CppUnitLite",
    ],
    "conditions": [
      ['OS=="win"', {
        }, {
          'cflags_cc': [ '-fexceptions' ]
        }
      ],
      ['OS=="win"', {
        "defines": ["ENABLE_REGEX"],
      }, {
      }]
    ],
    "xcode_settings": {
      "OTHER_CFLAGS": [ "-ObjC" ],
      "OTHER_CPLUSPLUSFLAGS" : ["-std=c++11", "-stdlib=libc++", "-v"],
      "OTHER_LDFLAGS": ["-stdlib=libc++"],
      "MACOSX_DEPLOYMENT_TARGET": "10.9",
      "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
      "ARCHS": ["x86_64"]
    },
    "cflags": [
      "-std=c++11"
    ]
  }]
}
