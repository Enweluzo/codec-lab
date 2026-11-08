#include "libavutil/frame.h"
#include "libavutil/common.h"
#include "libavutil/log.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

AVFrame *av_frame_alloc(Resolution *res, const FrameFormat format,
                        uint8_t byte_per_component) {
  unsigned plane_count = 0;
  switch (format) {
  case AV_PIX_FMT_RGB:
  case AV_PIX_FMT_YUV:
  case AV_PIX_FMT_YUV420P:
  case AV_PIX_FMT_YUV422P:
    plane_count = 3;
    break;

  case AV_PIX_FMT_RGBA:
    plane_count = 4;
    break;
  }

  unsigned frame_size = res->width * res->height; // unsampled
  AVFrame *frame = malloc(sizeof(AVFrame));
  frame->format = format;
  frame->byte_per_component = byte_per_component;

  bool sampling = format == AV_PIX_FMT_YUV420P || format == AV_PIX_FMT_YUV422P;

  if (!sampling) {
    uint8_t *buffer = malloc(plane_count * frame_size * byte_per_component);

    if (buffer == NULL) {
      log_oom();
      exit(EXIT_FAILURE);
    }

    for (int plane = 0; plane < plane_count; plane++) {
      frame->row_stride[plane] = res->width * byte_per_component;
      frame->data[plane] = buffer + (plane * frame_size * byte_per_component);
    }
  } else {
    unsigned sampled_plane_width = 0;
    unsigned sampled_plane_height = 0;

    if (format == AV_PIX_FMT_YUV422P)
      sampled_plane_width = ceil(res->width / 2.0);
    else if (format == AV_PIX_FMT_YUV420P) {

      sampled_plane_width = ceil(res->width / 2.0);
      sampled_plane_height = ceil(res->height / 2.0);
    }

    unsigned sampled_frame_size = sampled_plane_width * sampled_plane_height;
    unsigned sampled_plane_count = plane_count - 1;
    unsigned sampled_buffer_size = sampled_frame_size * sampled_plane_count;
    unsigned buffer_size =
        (frame_size + sampled_buffer_size) * byte_per_component;

    uint8_t *buffer = malloc(buffer_size);
    if (buffer == NULL) {
      log_oom();
      exit(EXIT_FAILURE);
    }

    frame->row_stride[0] =
        res->width * byte_per_component; // The first plane always full sampled
    frame->data[0] = buffer;

    uint8_t *sampled_buffer = buffer + frame_size;
    for (int sampled_plane = 0; sampled_plane < sampled_plane_count;
         sampled_plane++) {

      unsigned sampled_plane_index = sampled_plane + 1;

      frame->row_stride[sampled_buffer_size] =
          sampled_plane_width * byte_per_component;
      frame->data[sampled_plane_index] =
          sampled_buffer + (sampled_plane * sampled_frame_size);
    }
  }

  Resolution *_res = res;
  res = malloc(sizeof(Resolution));
  memcpy(res, _res, sizeof(Resolution));
  frame->res = res;
  return frame;
}

void av_frame_free(AVFrame *av_frame) {
  free(av_frame->data[0]);
  free(av_frame->res);
  free(av_frame);
}
