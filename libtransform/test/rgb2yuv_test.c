#include "libavutil/frame.h"
#include "libavutil/terminal.h"
#include "libtransform/rgb2yuv.h"
#include "libtransform/test/rgb2yuv.h"
#include <stdint.h>
#include <stdio.h>

/*
 * Test RGB-to-YUV conversion using a predefined 21x20 RGB frame.
 *
 * The RGB frame is converted to YUV using the BT.601 color standard,
 * then each output component is compared against predefined reference
 * YUV data generated from the same RGB frame using BT.601.
 *
 * The test passes only if the generated YUV frame matches the expected
 * reference data.
 */

void rgb2yuv_test() {
  const unsigned width = 21;
  const unsigned height = 20;
  AVFrame rgb_frame = {.plane_count = 3,
                       .res = {.width = width, .height = height},
                       .byte_per_component = 1,
                       .row_stride = {[0 ... 2] = width},
                       .format = AV_PIX_FMT_RGB,
                       .crop_bottom = 0,
                       .crop_top = 0,
                       .crop_left = 0,
                       .crop_right = 0};

  rgb_frame.data[0] = &rgb_frame_data[0][0];
  rgb_frame.data[1] = &rgb_frame_data[1][0];
  rgb_frame.data[2] = &rgb_frame_data[2][0];
  AVFrame *yuv_frame = rgb2yuv(&rgb_frame, AV_PIX_FMT_YUV, BT601, FullRange);

  bool test_passed = true;

  for (int plane = 0; plane < 3; ++plane) {
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        int gen_row_offset = width * y;
        int row_offset = yuv_frame->row_stride[plane] * y;
        int gen_offset = gen_row_offset + x;
        int offset = row_offset + x;

        int dif =
            yuv_frame_data[plane][gen_offset] - yuv_frame->data[plane][offset];
        test_passed = (dif == 0) || (dif == -1) || (dif == 1);
        if (!test_passed) {
          goto exit_check;
        }
      }
    }
  }

exit_check:

  av_frame_free(yuv_frame);

  if (test_passed) {
    printf(CSI_COLOR_GREEN "SUCCESS" CSI_COLOR_RESET
                           ": RGB -> YUV conversion\n");
  } else {
    printf(CSI_COLOR_RED "FAILED" CSI_COLOR_RESET ": RGB -> YUV conversion\n");
  }
}
