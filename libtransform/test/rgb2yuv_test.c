#include "libavutil/common.h"
#include "libavutil/frame.h"
#include "libavutil/sampling.h"
#include "libavutil/terminal.h"
#include "libtransform/rgb2yuv.h"
#include "test/color_samples.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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
  const int rgb_plane_count = 3;
  const int yuv_plane_count = 3;
  AVFrame *rgb_frame = av_frame_alloc(&res, NO_SAMPLING, AV_PIX_FMT_RGB);

  for (int i = 0; i < rgb_plane_count; i++) {
    memcpy(rgb_frame->data[i], &rgb_frame_data[i], 21 * 20);
  }

  AVFrame *yuv_frame = rgb2yuv(rgb_frame, &res, BT601, NO_SAMPLING, FullRange);

  bool test_passed = true;

  for (int i = 0; i < yuv_plane_count; i++) {
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

  av_frame_free(rgb_frame);
  av_frame_free(yuv_frame);

  if (test_passed) {
    printf(CSI_COLOR_GREEN "SUCCESS" CSI_COLOR_RESET
                           ": RGB -> YUV conversion test test\n");
  } else {
    printf(CSI_COLOR_RED "FAILED" CSI_COLOR_RESET
                         ": RGB -> YUV conversion test failed\n");
  }
}
