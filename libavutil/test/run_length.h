#ifndef RLE_TEST_H
#define RLE_TEST_H
#include <stdint.h>

extern void run_length_test();

// clang-format off
static int16_t coefficients[25] = {
     0,  15,  0,  0,  0,
     0,  0,  0,  0, -2,
     0,  0,  0,  0,  0,
     0,  0,  1,  0,  3,
     0,  0,  0,  0,  0
};

#endif
