#include "libavcodec/bitmap/bitmap.h"
#include "libavcodec/bitmap/create_image.h"
#include "libavcodec/bitmap/parse_image.h"
#include "libavutil/frame.h"
#include "libavutil/log.h"
#include "libavutil/terminal.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BITMAP_TEST_SRC_F "data/image.bmp"
#define BITMAP_TEST_GEN_F "data/image_gen.bmp"

void bitmap_test() {
  FILE *src_file = fopen(BITMAP_TEST_SRC_F, "rb");
  FILE *gen_file = fopen(BITMAP_TEST_GEN_F, "wb+");

  BitmapHeader *bitmap = read_bitmap_header(src_file);
  write_bitmap_header(bitmap, gen_file);

  AVFrame *frame = read_bitmap_frame(bitmap, src_file);
  write_bitmap_frame(bitmap, frame, gen_file);

  free(bitmap);
  av_frame_free(frame);
  fseek(src_file, 0, SEEK_SET);
  fseek(gen_file, 0, SEEK_SET);

  uint8_t *gen_buf = malloc(sizeof(uint8_t[500 << 10]));
  uint8_t *src_buf = malloc(sizeof(uint8_t[500 << 10]));
  if (gen_buf == NULL || src_buf == NULL) {
    log_oom();
    exit(EXIT_FAILURE);
  }

  bool test_passed = true;

  while (!feof(src_file)) {
    fread(gen_buf, sizeof(uint8_t), 500 << 10, src_file);
    fread(src_buf, sizeof(uint8_t), 500 << 10, gen_file);

    bool match = memcmp(gen_buf, src_buf, sizeof(uint8_t[500 << 10])) == 0;
    if (!match) {
      test_passed = false;
      break;
    }
  }
  if (test_passed) {
    printf(CSI_COLOR_GREEN "SUCCESS" CSI_COLOR_RESET ": Bitmap Parsing\n");
  } else {
    printf(CSI_COLOR_RED "FAILED" CSI_COLOR_RESET ": Bitmap Parsing\n");
  }

  fclose(src_file);
  fclose(gen_file);
}
