# ABPFilterParser.cpp

C++ Adblock Plus filter parser for lists like EasyList

This is a straight port of the prototype done in JS here:
https://github.com/bbondy/abp-filter-parser

It uses a bloom filter and Rabin-Karp algorithm to be super fast.

## Setup

```
npm install
```

## Running tests

```
make test
```

## Running sample

```
make
./run
```

## Clearing build files
```
make clean
```
