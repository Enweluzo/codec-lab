#ifndef BITMAP_H
#define BITMAP_H
#include "libavutil/common.h"

#define BITMAP_INFO_HEADER 40
#define BITMAP_HEADER_LEN 54

typedef struct {
  Resolution res;
  unsigned header_len;
  unsigned pixel_bytes;
} BitmapHeader;

#endif
