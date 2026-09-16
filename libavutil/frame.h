#ifndef AV_FRAME_H
#define AV_FRAME_H

#include "libavutil/common.h"
#include "libavutil/sampling.h"
#include <stdint.h>

typedef enum {
  AV_PIX_FMT_RGB,
  AV_PIX_FMT_RGBA,
  AV_PIX_FMT_YUV,
} FrameFormat;

typedef struct {
#define AV_NUM_DATA_POINTERS 4

  uint8_t *data[AV_NUM_DATA_POINTERS]; // Pointer to frame plane

  int row_stride[AV_NUM_DATA_POINTERS]; // Size of byte for each row

  FrameFormat format;

  int color_standard; // Defines the color standard for YUV conversion, e.g.
                      // BT.601, BT.709, or BT.2020.
} AVFrame;

extern AVFrame *av_frame_alloc(Resolution *res, SAMPLE sample,
                               const FrameFormat format);
extern void av_frame_free(AVFrame *av_frame);

#endif
