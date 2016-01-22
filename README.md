[![Build Status](https://travis-ci.org/bbondy/abp-filter-parser-cpp.svg?branch=master)](https://travis-ci.org/bbondy/abp-filter-parser-cpp)

# abp-filter-parser-cpp

Native node module, and C++ library for Adblock Plus filter parsing for lists like EasyList.

This is a straight port of the prototype done in JS here:
https://github.com/bbondy/abp-filter-parser

It uses a bloom filter and Rabin-Karp algorithm to be super fast.

## To include abp-filter-parser-cpp in your project:

```
npm install --save abp-filter-parser-cpp
```

## JS Sample

```javascript
var ABPFilterParser = ABPFilterParserLib.ABPFilterParser
var FilterOptions = ABPFilterParserLib.FilterOptions

var parser = new ABPFilterParser()
parser.parse('/public/ad/*$domain=slashdot.org')
parser.parse('/public/ad3/*$script')
var b1 = parser.matches('http://www.brianbondy.com/public/ad/some-ad', FilterOptions.script, 'slashdot.org')
var b2 = parser.matches('http://www.brianbondy.com/public/ad/some-ad', FilterOptions.script, 'digg.com')
console.log('public/ad/* should match b1.  Actual: ', b1)
console.log('public/ad/* should not match b2.  Actual: ', b2)
```

## C++ Sample

```c++
#include "ABPFilterParser.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

string getFileContents(const char *filename)
{
  ifstream in(filename, ios::in);
  if (in) {
    ostringstream contents;
    contents << in.rdbuf();
    in.close();
    return(contents.str());
  }
  throw(errno);
}

void writeFile(const char *filename, const char *buffer, int length)
{
  ofstream outFile(filename, ios::out | ios::binary);
  if (outFile) {
    outFile.write(buffer, length);
    outFile.close();
    return;
  }
  throw(errno);
}


int main(int argc, char**argv) {
  std::string &&easyListTxt = getFileContents("./test/data/easylist.txt");
  const char *urlsToCheck[] = {
    // ||pagead2.googlesyndication.com^$~object-subrequest
    "http://pagead2.googlesyndication.com/pagead/show_ads.js",
    // Should be blocked by: ||googlesyndication.com/safeframe/$third-party
    "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html",
    // Should be blocked by: ||googletagservices.com/tag/js/gpt_$third-party
    "http://www.googletagservices.com/tag/js/gpt_mobile.js",
    // Shouldn't be blocked
    "http://www.brianbondy.com"
  };

  // This is the site who's URLs are being checked, not the domain of the URL being checked.
  const char *currentPageDomain = "slashdot.org";

  // Parse easylist
  ABPFilterParser parser;
  parser.parse(easyListTxt.c_str());

  // Do the checks
  std::for_each(urlsToCheck, urlsToCheck + sizeof(urlsToCheck) / sizeof(urlsToCheck[0]), [&parser, currentPageDomain](std::string const &urlToCheck) {
    if (parser.matches(urlToCheck.c_str(), FONoFilterOption, currentPageDomain)) {
      cout << urlToCheck << ": You should block this URL!" << endl;
    } else {
      cout << urlToCheck << ": You should NOT block this URL!" << endl;
    }
  });

  int size;
  // This buffer is allocate on the heap, you must call delete[] when you're done using it.
  char *buffer = parser.serialize(size);
  writeFile("./ABPFilterParserData.dat", buffer, size);

  ABPFilterParser parser2;
  // Deserialize uses the buffer directly for subsequent matches, do not free until all matches are done.
  parser2.deserialize(buffer);
  // Prints the same as parser.matches would
  std::for_each(urlsToCheck, urlsToCheck + sizeof(urlsToCheck) / sizeof(urlsToCheck[0]), [&parser2, currentPageDomain](std::string const &urlToCheck) {
    if (parser2.matches(urlToCheck.c_str(), FONoFilterOption, currentPageDomain)) {
      cout << urlToCheck << ": You should block this URL!" << endl;
    } else {
      cout << urlToCheck << ": You should NOT block this URL!" << endl;
    }
  });
  delete[] buffer;
  return 0;
}
```

## Developing abp-filter-parser-cpp

1. Clone the git repository from GitHub:

    git clone --recursive abp-filter-parser-cpp

2. Open the working directory:

    cd abp-filter-parser-cpp

3. Install the Node (v5+) dependencies:

    npm install


## Make the node module

```
make
```

## Running sample (which also generates a .dat file for deserializing)

```
make sample
```

## Running tests

```
make test
```

## Clearing build files
```
make clean
```
