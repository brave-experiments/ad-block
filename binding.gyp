{
  "targets": [{
    "target_name": "test",
    "type": "executable",
    "sources": [
      "./test/test-main.cpp",
      "./test/parser-test.cpp",
      "./test/options-test.cpp",
      "./test/rule-types-test.cpp",
      "./test/util.cpp",
      "ABPFilterParser.cpp",
      "filter.cpp",
    ],
    "include_dirs": [
      ".",
      "src",
      "<!(node -e \"require('cppunitlite')\")",
      "<!(node -e \"require('nan')\")",
      "<!(node -e \"require('bloom-filter-cpp')\")"
    ],
    "dependencies": [
      "node_modules/cppunitlite/binding.gyp:CppUnitLite",
      "node_modules/bloom-filter-cpp/binding.gyp:test"
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
  }]
}
