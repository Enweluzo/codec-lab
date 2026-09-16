#ifndef RGB2YUV_H
#define RGB2YUV_H

#include "libavutil/frame.h"

#define FullRange 'F'  // luma && chroma = 0 - 255
#define LegalRange 'L' // luma = 16 - 235 && chroma = 16 - 240

// YUV color standard
#define BT601 0
#define BT709 1
#define BT2020 2

extern AVFrame *rgb2yuv(AVFrame *rgb_frame, Resolution *res, int color_standard,
                        SAMPLE sample, char range);

#endif
