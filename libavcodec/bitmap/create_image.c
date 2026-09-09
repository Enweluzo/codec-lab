#include "create_image.h"
#include "bitmap.h"
#include "libavutil/common.h"
#include "libavutil/cpu/endian.h"
#include "libavutil/frame.h"
#include "libavutil/log.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void write_bitmap_header(BitmapHeader *bitmap, FILE *file) {
  uint8_t header[BITMAP_HEADER_LEN] = {'B', 'M'};

  uint32_t *file_size = (void *)(header + 2);
  uint32_t *pixel_data_size = (void *)(header + 34);

  const unsigned row_pixel_bytes = bitmap->res.width * bitmap->pixel_bytes;
  const unsigned row_padding = (4 - (row_pixel_bytes % 4)) % 4;
  const unsigned bmp_row_stride = row_pixel_bytes + row_padding;

  *pixel_data_size = bmp_row_stride * bitmap->res.height;
  *file_size = *pixel_data_size + BITMAP_HEADER_LEN;

  *pixel_data_size = host_to_little_u32(pixel_data_size);
  *file_size = host_to_little_u32(file_size);

  uint32_t *pixel_data_offset = (void *)(header + 10);
  *pixel_data_offset = BITMAP_HEADER_LEN;
  *pixel_data_offset = host_to_little_u32(pixel_data_offset);

  uint32_t *info_header_size = (void *)(header + 14);
  *info_header_size = BITMAP_INFO_HEADER;
  *info_header_size = host_to_little_u32(info_header_size);

  uint32_t *width = (void *)(header + 18);
  uint32_t *height = (void *)(header + 22);
  *width = bitmap->res.width;
  *height = bitmap->res.height;
  *width = host_to_little_u32(width);
  *height = host_to_little_u32(height);

  uint16_t *planes = (void *)(header + 26);
  *planes = 1;
  *planes = host_to_little_u16(planes);

  uint16_t *pixel_bits = (void *)(header + 28);
  *pixel_bits = bitmap->pixel_bytes * 8;
  *pixel_bits = host_to_little_u32(pixel_bits);

  fwrite(header, sizeof(uint8_t), BITMAP_HEADER_LEN, file);
}

void write_bitmap_frame(BitmapHeader *bitmap, AVFrame *frame, FILE *file) {
  const unsigned row_pixel_bytes = bitmap->res.width * bitmap->pixel_bytes;
  const unsigned row_padding = (4 - (row_pixel_bytes % 4)) % 4;
  const unsigned bmp_row_stride = row_pixel_bytes + row_padding;

  uint8_t *row_buffer = calloc(bmp_row_stride, sizeof(uint8_t));
  if (row_buffer == NULL) {
    log_oom();
    exit(EXIT_FAILURE);
  }

  for (int y = 0; y < bitmap->res.height; y++) {
    /*
     * BMP stores rows bottom-to-top, so convert the
     * frame's top-to-bottom y coordinate to a BMP row.
     */
    unsigned bmp_row = (bitmap->res.height - 1) - y;
    unsigned bmp_row_offset = bmp_row * bmp_row_stride;
    unsigned file_offset = bitmap->header_len + bmp_row_offset;

    for (int channel = 0; channel < bitmap->pixel_bytes; channel++) {

      unsigned frame_row_offset = y * frame->row_stride[channel];

      /*
       * BMP stores color components in BGR order,
       * while the frame expects them in RGB order.
       */
      unsigned bmp_channel = (bitmap->pixel_bytes - 1) - channel;

      for (int x = 0; x < bitmap->res.width; x++) {

        unsigned pixel_offset = x * bitmap->pixel_bytes;
        row_buffer[pixel_offset + bmp_channel] =
            frame->data[channel][frame_row_offset + x];
      }

      fseek(file, file_offset, SEEK_SET);
      fwrite(row_buffer, sizeof(uint8_t), bmp_row_stride, file);
    }
  }

  free(row_buffer);
}
