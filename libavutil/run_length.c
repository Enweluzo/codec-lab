#include "libavutil/run_length.h"
#include "libavutil/log.h"
#include "libavutil/terminal.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ZeroRle *zero_run_encode(const int16_t *start, int len) {
  ZeroRlePair *encoded_buffer =
      malloc(sizeof(ZeroRlePair[len])); // Maximum capacity needed
  if (encoded_buffer == NULL) {
    log_oom();
    exit(EXIT_FAILURE);
  }
  int encoded_len = 0;

  for (int i = 0, run_count = 0; i < len; i++) {
    if (start[i] == 0) {
      run_count++;
    } else {
      encoded_buffer[encoded_len] =
          (ZeroRlePair){.run_count = run_count, .value = start[i]};
      encoded_len++;
      run_count = 0;
    }
  }

  encoded_buffer = realloc(encoded_buffer, sizeof(ZeroRle[encoded_len]));
  ZeroRle *zre = malloc(sizeof(ZeroRle));
  zre->length = encoded_len;
  zre->pair = encoded_buffer;
  return zre;
}

int16_t *zero_run_decode(const ZeroRle *encoded, int dest_len) {
  int16_t *decoded_buffer = calloc(dest_len, sizeof(int16_t));
  if (decoded_buffer == NULL) {
    log_oom();
    exit(EXIT_FAILURE);
  }

  for (int i = 0, decoded_len = 0; i < encoded->length; i++) {
    ZeroRlePair *pair = &encoded->pair[i];

    for (int z = 0; z < pair->run_count; z++) {
      if (decoded_len == dest_len) {
        // out of bound
        printf(CSI_COLOR_RED
               "Run length decoding error: decompressed length exceeds the "
               "expected buffer size\n" CSI_COLOR_RESET);
        abort();
      }
      decoded_buffer[decoded_len] = 0;
      decoded_len++;
    }

    if (decoded_len == dest_len) {
      // out of bound
      printf(CSI_COLOR_RED
             "Run length decoding error: decompressed length exceeds the "
             "expected buffer size\n" CSI_COLOR_RESET);
      abort();
    }
    decoded_buffer[decoded_len] = pair->value;
    decoded_len++;
  }

  return decoded_buffer;
}
