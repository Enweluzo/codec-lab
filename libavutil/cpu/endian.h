#ifndef ENDIAN_H
#define ENDIAN_H
#include <stdint.h>
#include <string.h>

#define LITTLE_E 'L'
#define BIG_E 'B'

static char Endianness = 0;
static inline char gh_endianness() {
  if (Endianness != 0) {
    return Endianness;
  }
  uint32_t n = 0xff;
  uint8_t *b = (void *)&n;
  if (b[0] == 0xff)
    Endianness = LITTLE_E;
  else
    Endianness = BIG_E;
  return Endianness;
}

/* ── Little endian ── */
static inline uint32_t little_to_host_u32(const void *src) {
  uint8_t n[4];
  memcpy(n, src, 4);
  if (gh_endianness() == LITTLE_E)
    return (uint32_t)n[0] | (uint32_t)n[1] << 8 | (uint32_t)n[2] << 16 |
           (uint32_t)n[3] << 24;
  return (uint32_t)n[3] << 24 | (uint32_t)n[2] << 16 | (uint32_t)n[1] << 8 |
         (uint32_t)n[0];
}
static inline uint32_t host_to_little_u32(const void *src) {
  return little_to_host_u32(src);
}

static inline uint16_t little_to_host_u16(const void *src) {
  uint8_t n[2];
  memcpy(n, src, 2);
  if (gh_endianness() == LITTLE_E)
    return (uint16_t)((uint16_t)n[0] | (uint16_t)n[1] << 8);
  return (uint16_t)((uint16_t)n[1] << 8 | (uint16_t)n[0]);
}

static inline uint16_t host_to_little_u16(const void *src) {
  return little_to_host_u16(src);
}

/* ── Big endian ── */
static inline uint32_t big_to_host_u32(const void *src) {
  uint8_t n[4];
  memcpy(n, src, 4);
  if (gh_endianness() == BIG_E)
    return (uint32_t)n[0] << 24 | (uint32_t)n[1] << 16 | (uint32_t)n[2] << 8 |
           (uint32_t)n[3];
  return (uint32_t)n[0] << 24 | (uint32_t)n[1] << 16 | (uint32_t)n[2] << 8 |
         (uint32_t)n[3];
}

static inline uint32_t host_to_big_u32(const void *src) {
  return big_to_host_u32(src);
}

static inline uint16_t big_to_host_u16(const void *src) {
  uint8_t n[2];
  memcpy(n, src, 2);
  if (gh_endianness() == BIG_E)
    return (uint16_t)((uint16_t)n[0] << 8 | (uint16_t)n[1]);
  return (uint16_t)((uint16_t)n[0] << 8 | (uint16_t)n[1]);
}

static inline uint16_t host_to_big_u16(const void *src) {
  return big_to_host_u16(src);
}

#endif
