{
  "targets": [{
    "target_name": "abp-filter-parser-cpp",
    "type": "static_library",
    "sources": [
      "ABPFilterParser.cpp",
      "ABPFilterParser.h",
      "filter.cpp",
      "filter.h",
    ],
    "include_dirs": [
      ".",
      "<!(node -e \"require('bloom-filter-cpp')\")"
    ],
    "dependencies": [
      "node_modules/bloom-filter-cpp/binding.gyp:bloom-filter-cpp"
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
  }, {
    "target_name": "sample",
    "type": "executable",
    "sources": [
      "main.cpp",
      "ABPFilterParser.cpp",
      "ABPFilterParser.h",
      "filter.cpp",
      "filter.h",
      "./node_modules/bloom-filter-cpp/BloomFilter.cpp",
      "./node_modules/bloom-filter-cpp/BloomFilter.h",
    ],
    "include_dirs": [
      ".",
      "<!(node -e \"require('bloom-filter-cpp')\")"
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
      "OTHER_LDFLAGS": ["-stdlib=libc++"],
      "MACOSX_DEPLOYMENT_TARGET": "10.9",
      "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
    },
  }, {
    "target_name": "perf",
    "type": "executable",
    "sources": [
      "perf.cpp",
      "ABPFilterParser.cpp",
      "ABPFilterParser.h",
      "filter.cpp",
      "filter.h",
      "./node_modules/bloom-filter-cpp/BloomFilter.cpp",
      "./node_modules/bloom-filter-cpp/BloomFilter.h",
    ],
    "include_dirs": [
      ".",
      "<!(node -e \"require('bloom-filter-cpp')\")"
    ],
    "defines": ["PERF_STATS"],
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
      "OTHER_LDFLAGS": ["-stdlib=libc++"],
      "MACOSX_DEPLOYMENT_TARGET": "10.9",
      "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
    },
  }, {
    "target_name": "test",
    "type": "executable",
    "sources": [
      "./test/test-main.cpp",
      "./test/parser-test.cpp",
      "./test/options-test.cpp",
      "./test/rule-types-test.cpp",
      "./test/util.cpp",
      "ABPFilterParser.cpp",
      "ABPFilterParser.h",
      "filter.cpp",
      "filter.h",
      "./node_modules/bloom-filter-cpp/BloomFilter.cpp",
      "./node_modules/bloom-filter-cpp/BloomFilter.h",
    ],
    "include_dirs": [
      ".",
      "<!(node -e \"require('cppunitlite')\")",
      "<!(node -e \"require('nan')\")",
      "<!(node -e \"require('bloom-filter-cpp')\")"
    ],
    "dependencies": [
      "node_modules/cppunitlite/binding.gyp:CppUnitLite",
    ],
    "conditions": [
      ['OS=="win"', {
        }, {
          'cflags_cc': [ '-fexceptions' ]
        }
      ],
      ['OS=="linux"', {
        "defines": ["DISABLE_REGEX"],
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
  }]
}
