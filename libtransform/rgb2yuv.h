#ifndef RGB2YUV_H
#define RGB2YUV_H

#include "libavutil/frame.h"

#define FullRange 'F'  // luma && chroma = 0 - 255
#define LegalRange 'L' // luma = 16 - 235 && chroma = 16 - 240

extern AVFrame *rgb2yuv(AVFrame *rgb_frame, FrameFormat format,
                        int color_standard, char range);

#endif
