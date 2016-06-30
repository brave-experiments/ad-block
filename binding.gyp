{
  "targets": [{
    "target_name": "abp-filter-parser-cpp",
    "sources": [
      "addon.cpp",
      "ABPFilterParserWrap.cpp",
      "ABPFilterParserWrap.h",
      "ABPFilterParser.cpp",
      "ABPFilterParser.h",
      "cosmeticFilter.cpp",
      "cosmeticFilter.h",
      "filter.cpp",
      "filter.h",
      "./node_modules/bloom-filter-cpp/BloomFilter.cpp",
      "./node_modules/bloom-filter-cpp/BloomFilter.h",
      "./node_modules/bloom-filter-cpp/hashFn.cpp",
      "./node_modules/bloom-filter-cpp/hashFn.h",
      "./node_modules/hashset-cpp/HashSet.cpp",
      "./node_modules/hashset-cpp/HashSet.h"
    ],
    "include_dirs": [
      ".",
      './node_modules/bloom-filter-cpp',
      './node_modules/hashset-cpp'
    ],
    "dependencies": [
    ],
    "conditions": [
      ['OS=="win"', {
        }, {
          'cflags_cc': [ '-fexceptions' ]
        }
      ]
    ],
    "xcode_settings": {
      "OTHER_CFLAGS": [ "-ObjC" ],
      "OTHER_CPLUSPLUSFLAGS" : ["-std=c++11","-stdlib=libc++", "-v"],
      "MACOSX_DEPLOYMENT_TARGET": "10.9",
      "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
    },
  }]
}
