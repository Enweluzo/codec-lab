#include "libtransform/rgb2yuv.h"
#include "libavutil/common.h"
#include "libavutil/frame.h"
#include "libavutil/sampling.h"
#include <math.h>
#include <stdint.h>

typedef const int (*coeff_row)[5];

/* Color space conversion coefficients for RGB -> YCbCr mapping.
 * Floating-point coefficients are scaled by 65536 to allow fixed-point
 * arithmetic.
 *
 * Entries are {
 *     Kr = Kr_f * 65536,
 *     Kg = Kg_f * 65536,
 *     Kb = Kb_f * 65536,
 *     Ku = scale_u * 65536,
 *     Kv = scale_v * 65536
 * }
 *
 * Kr_f + Kg_f + Kb_f = 1
 *
 * scale_u = 0.5 / (1 - Kb_f) // blue chrominance scaling
 * scale_v = 0.5 / (1 - Kr_f) // red chrominance scaling
 *
 * where:
 *     Yscaled = (Kr * R) + (Kg * G) + (Kb * B)
 *     Uscaled = Ku * (B - (Yscaled / 65536)) + (127.5 * 65536)
 *     Vscaled = Kv * (R - (Yscaled / 65536)) + (127.5 * 65536)
 *
 * The resulting YCbCr values are in the full 8-bit range.
 */
const uint64_t chroma_level_shift_q16 = 8355840;

const int rgb2yuv_coeffs[3][5] = {
    {19595, 38470, 7471, 36984, 46745}, // BT.601
    {13933, 46871, 4732, 35318, 41615}, // BT.709
    {17216, 44433, 3886, 34834, 44443}, // BT.2020
};

coeff_row get_coefficients(int color_standard) {
  return &rgb2yuv_coeffs[color_standard];
}

AVFrame *rgb2yuv(AVFrame *rgb_frame, Resolution *res, int color_standard,
                 SAMPLE sample, char range) {
  bool legal_range = range == LegalRange;

  coeff_row coeff = get_coefficients(color_standard);
  unsigned Kr = (*coeff)[0];
  unsigned Kg = (*coeff)[1];
  unsigned Kb = (*coeff)[2];
  unsigned Ku = (*coeff)[3];
  unsigned Kv = (*coeff)[4];

  unsigned yuv_plane_count = 3;
  AVFrame *yuv_frame = av_frame_alloc(res, sample, AV_PIX_FMT_YUV);

  /*
   * Process pixels in 2x2 blocks and average their chroma samples
   * when downsampling is required.
   *
   *   [x1][x2]       -> [avg]
   *   [y1][y2]
   *
   *   avg = (x1 + x2 + y1 + y2) / 4
   *
   * The exact sampling pattern depends on the selected chroma
   * subsampling mode (e.g. 4:2:0, 4:2:2).
   */
  for (int y = 0; y < res->height; y += 2) {

    for (int x = 0; x < res->width; x += 2) {
      uint8_t y_block[2][2] = {{0}};
      uint8_t u_block[2][2] = {{0}};
      uint8_t v_block[2][2] = {{0}};
      uint8_t (*(blocks[3]))[2][2] = {&y_block, &u_block, &v_block};
      for (int block_y = 0; block_y < 2; block_y++) {

        unsigned rgb_frame_row_offset =
            (y + block_y) * rgb_frame->row_stride[0];
        bool pass_y_limit = (y + block_y) >= res->height;

        for (int block_x = 0; block_x < 2; block_x++) {
          bool pass_x_limit = (x + block_x) >= res->width;

          if (pass_x_limit) {
            // Forward Fill the value
            y_block[block_y][block_x] = y_block[block_y][block_x - 1];
            u_block[block_y][block_x] = u_block[block_y][block_x - 1];
            v_block[block_y][block_x] = v_block[block_y][block_x - 1];
            break;
          }
          if (pass_y_limit) {
            // Forward Fill the value
            y_block[block_y][block_x] = y_block[block_y - 1][block_x];
            u_block[block_y][block_x] = u_block[block_y - 1][block_x];
            v_block[block_y][block_x] = v_block[block_y - 1][block_x];
            continue;
          }

          int R = rgb_frame->data[0][rgb_frame_row_offset + x + block_x];
          int G = rgb_frame->data[1][rgb_frame_row_offset + x + block_x];
          int B = rgb_frame->data[2][rgb_frame_row_offset + x + block_x];

          uint64_t y_q16 = (Kr * R) + (Kg * G) + (Kb * B);
          int Y = (y_q16 + (1ULL << 15)) >>
                  16; // round(y_q16 / (float)scaling_factor)

          uint64_t u_q32 = (((uint64_t)Ku * B) << 16) - (Ku * y_q16) +
                           (chroma_level_shift_q16 << 16);
          int U = (u_q32 + (1ULL << 31)) >> 32;

          uint64_t v_q32 = (((uint64_t)Kv * R) << 16) - (Kv * y_q16) +
                           (chroma_level_shift_q16 << 16);
          int V = (v_q32 + (1ULL << 31)) >> 32;

          if (legal_range) {
            Y = ((219 * Y + 127) / 255) + 16; // round((219 * Y / 255.0)) + 16
            U = ((224 * U + 127) / 255) + 16;
            V = ((224 * V + 127) / 255) + 16;
          }

          y_block[block_y][block_x] = Y;
          u_block[block_y][block_x] = U;
          v_block[block_y][block_x] = V;
        }
      }

      for (int i = 0; i < yuv_plane_count; i++) {
        uint8_t (*block)[2][2] = blocks[i];
        uint8_t *plane_data = yuv_frame->data[i];
        unsigned plane_row_stride = yuv_frame->row_stride[i];

        bool is_y_plane = i == 0;

        if (sample == NO_SAMPLING || is_y_plane) {
          for (int block_y = 0; block_y < 2; block_y++) {
            bool pass_y_limit = (y + block_y) >= res->height;
            if (pass_y_limit)
              break;

            for (int block_x = 0; block_x < 2; block_x++) {
              bool pass_x_limit = (x + block_x) >= res->width;
              if (pass_x_limit)
                break;

              unsigned row_offset = plane_row_stride * (y + block_y);
              plane_data[row_offset + x + block_x] = (*block)[block_y][block_x];
            }
          }
        } else if (sample == YUV422P) {
          for (int block_y = 0; block_y < 2; block_y++) {
            bool pass_y_limit = (y + block_y) >= res->height;
            if (pass_y_limit)
              break;

            unsigned sum = (*block)[block_y][0] + (*block)[block_y][1];
            uint8_t average = round(sum / 2.0);

            unsigned row_offset = plane_row_stride * (y + block_y);
            plane_data[row_offset + (x / 2)] = average;
          }
        } else if (sample == YUV420P) {
          unsigned sum =
              (*block)[0][0] + (*block)[0][1] + (*block)[1][0] + (*block)[1][1];
          uint8_t average = round(sum / 4.0);

          unsigned row_offset = plane_row_stride * (y / 2);
          plane_data[row_offset + (x / 2)] = average;
        }
      }
    }
  }

  return yuv_frame;
}
