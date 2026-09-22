#ifndef RLE_H
#define RLE_H

#include <stdint.h>

typedef struct {
  int run_count;
  int16_t value;
} ZeroRlePair;

typedef struct {
  ZeroRlePair *pair;
  int length;
} ZeroRle;

extern ZeroRle *zero_run_encode(const int16_t *start, int len);
extern int16_t *zero_run_decode(const ZeroRle *encoded, int dest_len);

#endif
