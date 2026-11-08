#include "libavutil/common.h"
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
  Resolution res = {.width = 21, .height = 20};
  AVFrame rgb_frame = {.plane_count = 3,
                       .res = &res,
                       .byte_per_component = 1,
                       .row_stride = {[0 ... 2] = 21},
                       .format = AV_PIX_FMT_RGB};

  rgb_frame.data[0] = &rgb_frame_data[0][0];
  rgb_frame.data[1] = &rgb_frame_data[1][0];
  rgb_frame.data[2] = &rgb_frame_data[2][0];
  AVFrame *yuv_frame = rgb2yuv(&rgb_frame, AV_PIX_FMT_YUV, BT601, FullRange);

  bool test_passed = true;

  for (int i = 0; i < 3; ++i) {
    for (int _i = 0; _i < 21 * 20; _i++) {
      uint8_t src = yuv_frame_data[i][_i];
      uint8_t gen = yuv_frame->data[i][_i];
      int error = gen - src;
      bool _pass = (error == 0) || (error == -1) || (error == 1);
      if (!_pass) {
        test_passed = false;
        break;
      }
    }
    if (!test_passed) {
      break;
    }
  }

  av_frame_free(yuv_frame);

  if (test_passed) {
    printf(CSI_COLOR_GREEN "SUCCESS" CSI_COLOR_RESET
                           ": RGB -> YUV conversion\n");
  } else {
    printf(CSI_COLOR_RED "FAILED" CSI_COLOR_RESET ": RGB -> YUV conversion\n");
  }
}
