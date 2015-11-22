#include "cosmeticFilter.h"
#include "hashFn.h"

uint64_t CosmeticFilter::hash() const {
  HashFn fn(19);
  return fn(data, strlen(data));
}

