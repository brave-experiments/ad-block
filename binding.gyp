{
  "targets": [{
    "target_name": "ad-block",
    "sources": [
      "addon.cc",
      "ad_block_client_wrap.cc",
      "ad_block_client_wrap.h",
      "ad_block_client.cc",
      "ad_block_client.h",
      "cosmetic_filter.cc",
      "cosmetic_filter.h",
      "filter.cc",
      "filter.h",
      "./cpp_modules/bloom-filter-cpp/BloomFilter.cpp",
      "./cpp_modules/bloom-filter-cpp/BloomFilter.h",
      "./cpp_modules/bloom-filter-cpp/hashFn.cpp",
      "./cpp_modules/bloom-filter-cpp/hashFn.h",
      "./cpp_modules/hashset-cpp/HashSet.cpp",
      "./cpp_modules/hashset-cpp/HashSet.h"
    ],
    "include_dirs": [
      ".",
      './cpp_modules/bloom-filter-cpp',
      './cpp_modules/hashset-cpp'
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
