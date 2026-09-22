#include "libavutil/run_length.h"
#include "libavutil/terminal.h"
#include "libavutil/test/run_length.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern void zero_run_length_test();

void run_length_test() { zero_run_length_test(); }

void zero_run_length_test() {
  const int coefficients_len = 25;
  ZeroRle *encoded = zero_run_encode(coefficients, coefficients_len);
  int16_t *decoded = zero_run_decode(encoded, coefficients_len);
  bool test_passed =
      memcmp(coefficients, decoded, sizeof(int16_t[coefficients_len])) == 0;

  if (test_passed) {
    printf(CSI_COLOR_GREEN "SUCCESS" CSI_COLOR_RESET
                           ": Run length encoding & decoding\n");
  } else {
    printf(CSI_COLOR_RED "FAILED" CSI_COLOR_RESET
                         ": Run length encoding & decoding\n");
  }
}
