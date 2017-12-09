{
  "targets": [{
    "target_name": "test",
    "type": "executable",
    "sources": [
      "../test/test_main.cc",
      "../test/parser_test.cc",
      "../test/options_test.cc",
      "../test/rule_types_test.cc",
      "../test/cosmetic_filter_test.cc",
      "../test/util.cc",
      "../ad_block_client.cc",
      "../ad_block_client.h",
      "../cosmetic_filter.cc",
      "../cosmetic_filter.h",
      "../filter.cc",
      "../filter.h",
      "../vendor/bloom-filter-cpp/BloomFilter.cpp",
      "../vendor/bloom-filter-cpp/BloomFilter.h",
      "../vendor/bloom-filter-cpp/hashFn.cpp",
      "../vendor/bloom-filter-cpp/hashFn.h",
      "../vendor/hashset-cpp/HashSet.cpp",
      "../vendor/hashset-cpp/HashSet.h"
    ],
    "include_dirs": [
      "..",
      '../vendor/bloom-filter-cpp',
      '../vendor/hashset-cpp',
      '../vendor/CppUnitLite',
      '../node_modules/nan'
      "..",
    ],
    "dependencies": [
      "../vendor/CppUnitLite/binding.gyp:CppUnitLite",
    ],
    "conditions": [
      ['OS=="win"', {
        }, {
          'cflags_cc': [ '-fexceptions' ]
        }
      ],
      ['OS!="linux"', {
        "defines": ["ENABLE_REGEX"],
      }, {
      }]
    ],
    "xcode_settings": {
      "OTHER_CFLAGS": [ "-ObjC" ],
      "OTHER_CPLUSPLUSFLAGS" : ["-std=c++11","-stdlib=libc++", "-v"],
      "OTHER_LDFLAGS": ["-stdlib=libc++"],
      "MACOSX_DEPLOYMENT_TARGET": "10.9",
      "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
    },
    "cflags": [
      "-std=c++11"
    ]
  }]
}
