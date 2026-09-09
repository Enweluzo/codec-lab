#ifndef CREATE_BITMAP_IMAGE_H
#define CREATE_BITMAP_IMAGE_H
#include "bitmap.h"
#include "libavutil/frame.h"
#include <stdint.h>
#include <stdio.h>

extern void write_bitmap_header(BitmapHeader *bh, FILE *fp);
extern void write_bitmap_frame(BitmapHeader *bh, AVFrame *av_frame, FILE *fp);

#endif
