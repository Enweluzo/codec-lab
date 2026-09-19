#ifndef DCT_H
#define DCT_H

#include <stdint.h>
extern void level_shift_64(uint8_t *src, int8_t *dest, int8_t shift);
int16_t *dct_8(uint8_t *_block);

#endif
