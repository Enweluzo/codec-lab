#ifndef DCT_PATTERN_H
#define DCT_PATTERN_H

#include <stdint.h>
typedef int32_t v_int32_t __attribute__((vector_size(sizeof(int32_t[64]))));
/*
 * pattern[v][u] = cos[ (π / N) (x + 0.5) u ] × cos[ (π / M) (y + 0.5) v ]
 * where x = 0, 1, ..., N-1
 * and y = 0,1, ...,M-1
 */

static const int32_t C_0 = 0.125 * (1ULL << 16);
static const int32_t C_1 = 0.1768 * (1ULL << 16);
static const int32_t C_2 = 0.25 * (1ULL << 16);

// Scaled by 2^16
extern const v_int32_t pattern_8[8][8];

#endif
