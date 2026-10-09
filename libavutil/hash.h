#ifndef HASH_H
#define HASH_H
#include <stddef.h>
#include <stdint.h>

extern uint32_t murmurHash3(uint32_t seed, void *src, size_t size);

#endif
