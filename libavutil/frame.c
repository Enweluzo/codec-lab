#include "libavutil/frame.h"
#include "libavutil/common.h"
#include "libavutil/log.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

AVFrame *av_frame_alloc(unsigned width, unsigned height,
                        const FrameFormat format, uint8_t byte_per_component) {
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

  unsigned width_padding = (64 - width % 64) % 64;
  unsigned height_padding = (64 - height % 64) % 64;

  AVFrame *frame = malloc(sizeof(AVFrame));
  frame->res.width = width + width_padding;
  frame->res.height = height + height_padding;
  frame->format = format;
  frame->byte_per_component = byte_per_component;
  frame->crop_right = width_padding;
  frame->crop_bottom = height_padding;
  frame->crop_left = 0;
  frame->crop_top = 0;

  bool sampling = format == AV_PIX_FMT_YUV420P || format == AV_PIX_FMT_YUV422P;

  if (!sampling) {
    unsigned frame_size = frame->res.width * frame->res.height;
    uint8_t *buffer = malloc(plane_count * frame_size * byte_per_component);

    if (buffer == NULL) {
      log_oom();
      exit(EXIT_FAILURE);
    }

    for (int plane = 0; plane < plane_count; plane++) {
      frame->row_stride[plane] = frame->res.width * byte_per_component;
      frame->data[plane] = buffer + (plane * frame_size * byte_per_component);
    }
  } else {
    unsigned sampled_width = 0;
    unsigned sampled_height = 0;

    if (format == AV_PIX_FMT_YUV422P) {
      sampled_width = width / 2;
      sampled_height = height;
    } else if (format == AV_PIX_FMT_YUV420P) {

      sampled_width = width / 2;
      sampled_height = height / 2;
    }
    unsigned sampled_padding_width = (64 + sampled_width % 64) % 64;
    unsigned sampled_padding_height = (64 + sampled_height % 64) % 64;
    sampled_width = sampled_width + sampled_padding_width;
    sampled_height = sampled_height + sampled_padding_height;

    unsigned frame_size = frame->res.width * frame->res.height;
    unsigned sampled_frame_size = sampled_width * sampled_height;
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
        frame->res.width *
        byte_per_component; // The first plane always full sampled
    frame->data[0] = buffer;

    uint8_t *sampled_buffer = buffer + frame_size;
    for (int sampled_plane = 0; sampled_plane < sampled_plane_count;
         sampled_plane++) {

      unsigned sampled_plane_index = sampled_plane + 1;

      frame->row_stride[sampled_plane_index] =
          sampled_width * byte_per_component;
      frame->data[sampled_plane_index] =
          sampled_buffer + (sampled_plane * sampled_frame_size);
    }
  }

  return frame;
}

void av_frame_free(AVFrame *av_frame) {
  free(av_frame->data[0]);
  free(av_frame);
}
