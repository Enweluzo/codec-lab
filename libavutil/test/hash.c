#include "libavutil/hash.h"
#include "libavutil/terminal.h"
#include <stdint.h>
#include <stdio.h>

void hash_test() {
  uint32_t result_0 = murmurHash3(10, "hello world", 11);
  uint32_t result_1 = murmurHash3(10, "Codec lab", 9);

  bool test_passed = result_0 == 2980371087 && result_1 == 3176057658;

  if (test_passed) {
    printf(CSI_COLOR_GREEN "SUCCESS" CSI_COLOR_RESET ": MurmurHash3\n");
  } else {
    printf(CSI_COLOR_RED "FAILED" CSI_COLOR_RESET ": MurmurHash3\n");
  }
}
