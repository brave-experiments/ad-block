[![Build Status](https://travis-ci.org/bbondy/abp-filter-parser-cpp.svg?branch=master)](https://travis-ci.org/bbondy/abp-filter-parser-cpp)

# ABPFilterParser.cpp

C++ Adblock Plus filter parser for lists like EasyList

This is a straight port of the prototype done in JS here:
https://github.com/bbondy/abp-filter-parser

It uses a bloom filter and Rabin-Karp algorithm to be super fast.

## Setup

```
npm install --save abp-filter-parser-cpp
```


## Sample

```c++
#include "ABPFilterParser.h"
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
  const char *urlToCheck = "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html";

  // This is the site who's URLs are being checked, not the domain of the URL being checked.
  const char *currentPageDomain = "slashdot.org";

  // Parse easylist
  ABPFilterParser parser;
  parser.parse(easyListTxt.c_str());

  // Do a check
  if (parser.matches(urlToCheck, FONoFilterOption, currentPageDomain)) {
    cout << "You should block this URL!" << endl;
  } else {
    cout << "You should NOT block this URL!" << endl;
  }

  int size;
  char *buffer = parser.serialize(size);
  writeFile("./ABPFilterParserData.dat", buffer, size);

  // Note that this buffer must be freed and will actually be used directly by the deserializer.
  ABPFilterParser parser2;
  parser2.deserialize(buffer);
  // Prints the same as parser.matches would
  if (parser2.matches(urlToCheck, FONoFilterOption, currentPageDomain)) {
    cout << "You should block this URL!" << endl;
  } else {
    cout << "You should NOT block this URL!" << endl;
  }
  delete[] buffer;
  return 0;
}
```
## Build everything in release

```
make
```

## Build everything in debug

```
make build-debug
```

## Running sample

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
