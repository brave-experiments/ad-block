{
  "targets": [{
    "target_name": "test",
    "type": "executable",
    "sources": [
      "../test/test-main.cpp",
      "../test/parser-test.cpp",
      "../test/options-test.cpp",
      "../test/rule-types-test.cpp",
      "../test/cosmetic-filter-test.cpp",
      "../test/util.cpp",
      "../ABPFilterParser.cpp",
      "../ABPFilterParser.h",
      "../cosmeticFilter.cpp",
      "../cosmeticFilter.h",
      "../filter.cpp",
      "../filter.h",
      "../node_modules/bloom-filter-cpp/BloomFilter.cpp",
      "../node_modules/bloom-filter-cpp/BloomFilter.h",
      "../node_modules/bloom-filter-cpp/hashFn.cpp",
      "../node_modules/bloom-filter-cpp/hashFn.h",
      "../node_modules/hashset-cpp/HashSet.cpp",
      "../node_modules/hashset-cpp/HashSet.h"
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
