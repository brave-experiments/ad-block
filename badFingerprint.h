#pragma once

#include "math.h"
#include <string.h>
#include "HashSet.h"

#ifdef PERF_STATS
#include <fstream>
#include <iostream>
using namespace std;
#endif

class BadFingerprint {
public:
  uint64_t hash() const {
    return 0;
  }

  ~BadFingerprint() {
    if (data) {
      delete[] data;
    }
  }
  BadFingerprint(const char *data) {
    this->data = new char[strlen(data) + 1];
    strcpy(this->data, data);
  }

  BadFingerprint(const BadFingerprint &rhs) {
    data = new char[strlen(rhs.data) + 1];
    memcpy(data, rhs.data, strlen(rhs.data) + 1);
  }

  BadFingerprint() : data(nullptr) {
  }

  bool operator==(const BadFingerprint &rhs) const {
    return !strcmp(data, rhs.data);
  }

  bool operator!=(const BadFingerprint &rhs) const {
    return !(*this == rhs);
  }

  uint32_t serialize(char *buffer) {
    if (buffer) {
      memcpy(buffer, data, strlen(data) + 1);
    }
    return strlen(data) + 1;
  }

  uint32_t deserialize(char *buffer, uint32_t bufferSize) {
    int len = strlen(buffer);
    data = new char[len + 1];
    memcpy(data, buffer, len + 1);
    return len + 1;
  }

  char *data;
};

class BadFingerprintsHashSet : public HashSet<BadFingerprint> {
public:
  BadFingerprintsHashSet() : HashSet<BadFingerprint>(1) {
  }

  void generateHeader(const char *filename) {
#ifdef PERF_STATS
    ofstream outFile;
    outFile.open(filename);

    outFile << "#pragma once\n";
    outFile << "/**\n  *\n  * Auto generated bad filters\n  */\n";
    outFile << "const char *badFingerprints[] = {\n";
    for (uint32_t bucketIndex = 0; bucketIndex < bucketCount; bucketIndex++) {

      HashItem<BadFingerprint> *hashItem = buckets[bucketIndex];
      while (hashItem) {
        BadFingerprint *badFingerprint = hashItem->hashItemStorage;
        outFile << "\"" << badFingerprint->data << "\"," << endl;
        hashItem = hashItem->next;
      }
    }
    outFile << "};\n" << endl;
    outFile << "const char *badSubstrings[] = {\"http\", \"www\" };" <<endl;
    outFile.close();
    #endif
  }

};
