#include "libavutil/huffman.h"
#include "libavutil/terminal.h"
#include "libavutil/test/huffman.h"
#include <stdio.h>

void get_huffman_code_test() {
  HuffmanCode *code = get_huffman_code(huff_data, 100);
  bool test_passed = code[0].code == 0b0 && code[128].code == 0b111 &&
                     code[255].code == 0b101 && code[10].code == 0b100 &&
                     code[7].code == 0b1101 && code[64].code == 0b1100;

  if (test_passed)
    printf(CSI_COLOR_GREEN "SUCCESS" CSI_COLOR_RESET ": Get Huffman Code\n");
  else
    printf(CSI_COLOR_GREEN "FAILED" CSI_COLOR_RESET ": Get Huffman Code\n");
}
