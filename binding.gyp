{
  "targets": [{
    "target_name": "abp-filter-parser-cpp",
    "type": "static_library",
    "sources": [
      "ABPFilterParser.cpp",
      "ABPFilterParser.h",
      "cosmeticFilter.cpp",
      "cosmeticFilter.h",
      "filter.cpp",
      "filter.h",
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
