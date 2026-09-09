#ifndef PARSE_BITMAP_IMAGE_H
#define PARSE_BITMAP_IMAGE_H
#include "bitmap.h"
#include "libavutil/frame.h"
#include "stdio.h"
#include <stdint.h>

extern BitmapHeader *read_bitmap_header(FILE *fp);
extern AVFrame *read_bitmap_frame(BitmapHeader *bh, FILE *fp);
#endif
