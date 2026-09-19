
#include "libavutil/dct/_test_sample.h"
#include "libavutil/dct/dct.h"
#include "libavutil/terminal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern void dct8_test();

void dct_test() { dct8_test(); }

void dct8_test() {
  int16_t *result = dct_8(sample);
  bool match = memcmp(result, sample_dct, sizeof(int16_t[64])) == 0;
  if (match) {
    printf(CSI_COLOR_GREEN "SUCCESS" CSI_COLOR_RESET ": DCT 8 Parsing test\n");
  } else {
    printf(CSI_COLOR_RED "FAILED" CSI_COLOR_RESET
                         ": DCT 8 Parsing test failed\n");
  }
}
