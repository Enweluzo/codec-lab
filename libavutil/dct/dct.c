#include "dct.h"
#include "libavutil/dct/pattern.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t v_int8_t __attribute__((vector_size(64)));

int16_t *dct_8(uint8_t *_block) {
  int8_t block[64];
  level_shift_64(_block, block, -128);

  int16_t *coefficient = malloc(sizeof(int16_t[64]));

  const unsigned row_stride = 8;

  for (int v = 0; v < 8; v++) {
    unsigned row_offset = row_stride * v;
    for (int u = 0; u < 8; u++) {
      v_int32_t vec_result = {0};
      for (int i = 0; i < 64; i++) {
        vec_result[i] = block[i];
      }

      v_int32_t vec_pattern =
          pattern_8[v][u]; // has been fixed point scaled by 2^16
      vec_result *= vec_pattern;

      int64_t coef = 0;
      for (int i = 0; i < 64; i++) {
        coef += vec_result[i];
      }

      if (u == 0 && v == 0) {
        coef = C_0 * coef;
      } else if (u == 0 || v == 0) {
        coef = C_1 * coef;
      } else {
        coef = C_2 * coef;
      }

      coefficient[row_offset + u] = (coef + (1ULL << 31)) >> 32;
    }
  }
  return coefficient;
}

void level_shift_64(uint8_t *src, int8_t *dest, int8_t shift) {
  v_int8_t vec;
  memcpy(&vec, src, 64);
  vec += shift;
  memcpy(dest, &vec, 64);
}
