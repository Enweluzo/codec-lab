/*
 * Entry point to all the test
 * Note : not an actual test for main
 */
#include "libavcodec/bitmap/_test.h"
#include "libavutil/dct/_test.h"
#include "libavutil/test/run_length.h"
#include "libtransform/test/rgb2yuv.h"

void main_test() {
  bitmap_test();
  rgb2yuv_test();
  dct_test();
  run_length_test();
}
