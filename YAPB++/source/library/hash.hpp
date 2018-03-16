#ifndef QUICK_HASH_HPP_YIA
#define QUICK_HASH_HPP_YIA

#include <stdint.h>

#if 0

#include "fnv_hash.hpp"

#else

// 64-bit mixing function

uint64_t quick_hash(uint64_t key)
{
    //return key + key*key;
  key = (~key) + (key << 21); // key = (key << 21) - key - 1;
  key = key ^ (key >> 24);
  key = (key + (key << 3)) + (key << 8); // key * 265
  key = key ^ (key >> 14);
  key = (key + (key << 2)) + (key << 4); // key * 21
  key = key ^ (key >> 28);
  key = key + (key << 31);
  return key;
}

uint64_t inline hash_combine(uint64_t val1, uint64_t val2)
{
    return (uint64_t)15485863 * val1 + (uint64_t)19661 * val2;
}

uint64_t inline hash_combine(uint64_t val1, uint64_t val2, uint64_t val3)
{
    return (uint64_t)15485863 * val1 +
           (uint64_t)19661 * val2 +
           (uint64_t)22801763489 * val3;
}
#endif

#endif
