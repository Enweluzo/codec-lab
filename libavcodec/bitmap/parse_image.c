/* Bitmap Image Parsing
 *
 * Note: support is limited to BITMAP_INFO_HEADER's and has no support
 * for bitmap files that includes a color pallet
 */
#include "parse_image.h"
#include "bitmap.h"
#include "libavutil/cpu/endian.h"
#include "libavutil/frame.h"
#include "libavutil/log.h"
#include "libavutil/sampling.h"
#include "libavutil/terminal.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BitmapHeader *read_bitmap_header(FILE *file) {
  const unsigned header_buffer_size = 30;
  uint8_t header[header_buffer_size];

  fread(header, sizeof(uint8_t), header_buffer_size, file);

  char expected_magic_number[2] = {'B', 'M'};
  bool valid_magic_number =
      memcmp(expected_magic_number, header, sizeof(expected_magic_number)) == 0;

  if (!valid_magic_number) {
    printf(CSI_COLOR_RED
           "Invalid file format expected a bitmap image" CSI_COLOR_RESET "\n");
    exit(EXIT_FAILURE);
  }

  uint32_t info_header_size = little_to_host_u32(header + 14);

  if (info_header_size != BITMAP_INFO_HEADER) {
    printf(CSI_COLOR_RED "Support for Bitmap file header is limited to "
                         "BITMAP_INFO_HEADER" CSI_COLOR_RESET "\n");
    exit(EXIT_FAILURE);
  }

  Resolution resolution = {
      .width = little_to_host_u32(header + 18),
      .height = little_to_host_u32(header + 22),
  };

  unsigned pixel_data_offset = little_to_host_u32(header + 10);
  unsigned bits_per_pixel = little_to_host_u16(header + 28);

  bool supported_pixel_format = bits_per_pixel == 24 || bits_per_pixel == 32;

  if (!supported_pixel_format) {
    printf(CSI_COLOR_RED "Support for Bitmap file does not include parsing "
                         "color palette" CSI_COLOR_RESET "\n");
    exit(EXIT_FAILURE);
  }

  BitmapHeader *bitmap = malloc(sizeof(BitmapHeader));

  bitmap->res = resolution;
  bitmap->header_len = pixel_data_offset;
  bitmap->pixel_bytes = bits_per_pixel / 8;

  return bitmap;
}

AVFrame *read_bitmap_frame(BitmapHeader *bitmap, FILE *file) {
  const unsigned row_pixel_bytes = bitmap->res.width * bitmap->pixel_bytes;
  const unsigned row_padding = (4 - (row_pixel_bytes % 4)) % 4;
  const unsigned bmp_row_stride = row_pixel_bytes + row_padding;

  uint8_t *row_buffer = malloc(sizeof(uint8_t[row_pixel_bytes]));
  if (row_buffer == NULL) {
    log_oom();
    exit(EXIT_FAILURE);
  }

  FrameFormat format;
  switch (bitmap->pixel_bytes) {
  case 3:
    format = AV_PIX_FMT_RGB;
    break;
  case 4:
    format = AV_PIX_FMT_RGBA;
    break;
  }

  AVFrame *frame = av_frame_alloc(&bitmap->res, NO_SAMPLING, format);

  for (int y = 0; y < bitmap->res.height; y++) {
    /*
     * BMP stores rows bottom-to-top, so convert the
     * frame's top-to-bottom y coordinate to a BMP row.
     */
    unsigned bmp_row = (bitmap->res.height - 1) - y;
    unsigned bmp_row_offset = bmp_row * bmp_row_stride;
    unsigned file_offset = bitmap->header_len + bmp_row_offset;

    fseek(file, file_offset, SEEK_SET);
    fread(row_buffer, sizeof(uint8_t), row_pixel_bytes, file);

    for (int channel = 0; channel < bitmap->pixel_bytes; channel++) {

      unsigned frame_row_offset = y * frame->row_stride[channel];

      /*
       * BMP stores color components in BGR order,
       * while the frame expects them in RGB order.
       */
      unsigned bmp_channel = (bitmap->pixel_bytes - 1) - channel;

      for (int x = 0; x < bitmap->res.width; x++) {

        unsigned pixel_offset = x * bitmap->pixel_bytes;
        frame->data[channel][frame_row_offset + x] =
            row_buffer[pixel_offset + bmp_channel];
      }
    }
  }
  free(row_buffer);

  return frame;
}
