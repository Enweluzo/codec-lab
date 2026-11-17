#ifndef BITMAP_H
#define BITMAP_H

#define BITMAP_INFO_HEADER 40
#define BITMAP_HEADER_LEN 54

typedef struct {
  unsigned width;
  unsigned height;
  unsigned header_len;
  unsigned pixel_bytes;
} BitmapHeader;

#endif
