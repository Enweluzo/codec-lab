#include "libavutil/cpu/endian.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

uint32_t murmurHash3(uint32_t seed, const void *_src, size_t size) {
  const uint8_t *src = _src;
  uint32_t h = seed;

  int nblock = size / 4;
  for (size_t block_index = 0; block_index < nblock; ++block_index) {
    const uint8_t *target_src = src + block_index * 4;
    uint32_t block = 0;

    block = little_to_host_u32(target_src);

    block *= 0xcc9e2d51;
    block = (block << 15) | (block >> (32 - 15));
    block *= 0x1b873593;
    h ^= block;
    h = (h << 13) | (h >> (32 - 13));
    h = h * 5 + 0xe6546b64;
  }

  int trail_len = size - nblock * 4;
  if (trail_len) {
    const uint8_t *target_src = src + nblock * 4;
    ;
    uint32_t trail_block = 0;
    for (int trail_index = 0; trail_index < trail_len; ++trail_index) {
      trail_block |= (uint32_t)target_src[trail_index] << trail_index * 8;
    }
    trail_block *= 0xcc9e2d51;
    trail_block = (trail_block << 15) | (trail_block >> (32 - 15));
    trail_block *= 0x1b873593;
    h ^= trail_block;
  }

  h ^= (uint32_t)size;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}

// static inline uint32_t rotl32(uint32_t x, int r) {
//   return (x << r) | (x >> (32 - r));
// }

// uint32_t murmurHash3(uint32_t seed, const void *_src, size_t size) {
// const uint8_t *src = _src;
// uint32_t h = seed;
// const uint32_t c1 = 0xcc9e2d51, c2 = 0x1b873593;

// size_t nblocks = size / 4;
// for (size_t b = 0; b < nblocks; b++) {
//   uint32_t k = little_to_host_u32(src + b * 4);
//   k *= c1;
//   k = rotl32(k, 15);
//   k *= c2;
//   h ^= k;
//   h = rotl32(h, 13);
//   h = h * 5 + 0xe6546b64;
// }

// const uint8_t *tail = src + nblocks * 4;
// uint32_t k = 0;
// switch (size & 3) {
// case 3:
//   k ^= (uint32_t)tail[2] << 16; /* fallthrough */
// case 2:
//   k ^= (uint32_t)tail[1] << 8; /* fallthrough */
// case 1:
//   k ^= tail[0];
//   k *= c1;
//   k = rotl32(k, 15);
//   k *= c2;
//   h ^= k;
// }

// h ^= (uint32_t)size;
// h ^= h >> 16;
// h *= 0x85ebca6b;
// h ^= h >> 13;
// h *= 0xc2b2ae35;
// h ^= h >> 16;
// return h;
// }
