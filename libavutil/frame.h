#ifndef AV_FRAME_H
#define AV_FRAME_H

#include "libavutil/common.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
  AV_PIX_FMT_RGB,
  AV_PIX_FMT_RGBA,
  AV_PIX_FMT_YUV,
  AV_PIX_FMT_YUV422P,
  AV_PIX_FMT_YUV420P
} FrameFormat;

typedef struct {
#define AV_NUM_DATA_POINTERS 4
  uint8_t plane_count;

  uint8_t *data[AV_NUM_DATA_POINTERS]; // Pointer to frame plane

  int row_stride[AV_NUM_DATA_POINTERS]; // Size of byte for each row

  FrameFormat format;

  Resolution res;

  /**
   * The number of pixels to discard from the
   * top/bottom/left/right border of the frame to obtain the sub-rectangle of
   * the frame intended for presentation.
   */
  size_t crop_top;
  size_t crop_bottom;
  size_t crop_left;
  size_t crop_right;

  uint8_t
      byte_per_component; // Number of bytes used to store each pixel components

#define BT601 0
#define BT709 1
#define BT2020 2
  int color_standard; // Defines the color standard for YUV conversion, e.g.
                      // BT.601, BT.709, or BT.2020.
} AVFrame;

extern AVFrame *av_frame_alloc(unsigned width, unsigned height,
                               const FrameFormat format,
                               uint8_t byte_per_component);
extern void av_frame_free(AVFrame *av_frame);

#endif
