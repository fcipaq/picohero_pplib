/*
 * pplib - a library for the Pico Hero handheld
 *
 * Copyright (C) 2023 Daniel Kammer (daniel.kammer@web.de)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma GCC optimize("Ofast")

#include "blitter.h"
#include "gbuffers.h"
#include "hardware/interp.h"

#include <Arduino.h>

/*
 * Blits a source buffer to a destination buffer
 * alpha must equal either BLIT_NO_ALPHA (no alpha channel) or the
 * color that shall be transparent
 */
void blit_buf(coord_t kx,       // coordinates of upper left corner
              coord_t ky,
              int32_t alpha,
              gbuffer_t src,    // pointer to source buffer
              gbuffer_t dst) {  // pointer to destination buffer

  uint16_t srcBufWidth = gbuf_get_width(src);
  uint16_t srcBufHeight = gbuf_get_height(src);
  uint16_t dstBufWidth = gbuf_get_width(dst);
  uint16_t dstBufHeight = gbuf_get_height(dst);

  // area is out of destination buffer
  if (kx >= dstBufWidth || (kx + srcBufWidth < 0) || ky >= dstBufHeight || (ky + srcBufHeight < 0))
    return;

  // check if the blitting area needs to be cropped
  uint16_t start_x = 0;
  uint16_t start_y = 0;
  uint16_t end_x = 0;
  uint16_t end_y = 0;

  uint32_t cpybuf_s = 0;

  if (kx < 0) {
    start_x = 0;
    cpybuf_s = -kx;
  } else {
    start_x = kx;
  }

  if (ky < 0) {
    start_y = 0;
    cpybuf_s += (-ky) * srcBufWidth;
  } else {
    start_y = ky;
  }

  if (kx + srcBufWidth > dstBufWidth)
    end_x = dstBufWidth;
  else
    end_x = kx + srcBufWidth;

  if (ky + srcBufHeight > dstBufHeight)
    end_y = dstBufHeight;
  else
    end_y = ky + srcBufHeight;

  // prepare start values
  uint32_t cpybuf_d = start_y * dstBufWidth + start_x;

  // copy the buffer
  if (alpha == BLIT_NO_ALPHA) {
    for (coord_t y = start_y; y < end_y; y++) {
      for (coord_t x = start_x; x < end_x; x++) {
        dst.data[cpybuf_d] = src.data[cpybuf_s];

        cpybuf_s++;
        cpybuf_d++;
      }
      cpybuf_s += srcBufWidth - (end_x - start_x);
      cpybuf_d += dstBufWidth - (end_x - start_x);
    }
  } else {
    for (coord_t y = start_y; y < end_y; y++) {
      for (coord_t x = start_x; x < end_x; x++) {
        if (src.data[cpybuf_s] != alpha)
          dst.data[cpybuf_d] = src.data[cpybuf_s];

        cpybuf_s++;
        cpybuf_d++;
      }
      cpybuf_s += srcBufWidth - (end_x - start_x);
      cpybuf_d += dstBufWidth - (end_x - start_x);
    }
  }

}  // blitBuf

/*
 * Blits a source buffer to a destination buffer
 * while rotating and/or zooming it.
 * alpha must equal either BLIT_NO_ALPHA (no alpha channel) or the
 * color that shall be transparent
 */
// Blits a buffer with rotation and zooming using HW acceleration of the interpolater
// Note due to the nature of rotating with the interpolator, when rotating the original
// image must not fill out whole buffer. In fact it needs to be smaller by a factor of
// sqrt(2) * sqrt(1.75). This is because otherwise there will be artifacts from the
// interpolator folding back parts of the image into the visible area.
// For example if the buffer is 64 by 32 pixels, the visible area must be centered and
// not exceed 34 by 12 pixels. This is somewhat a tradeoff between memory usage and
// performance. Images are therefore best stored in flash.
// Image size must be power of 2 and the image must not exeed the length of the conatiner
// by a factor of 1 / sqrt(2) / sqrt(1.75)

void blit_buf(coord_t kx,       // x-coord where to blit the of CENTER of the image
              coord_t ky,       // y-coord where to blit the of CENTER of the image
              float zoom,       // zoom factor (same in both directions)
              float rot,        // rotation of the image (in rad)
              int32_t alpha,    // color which is NOT being drawn (BLIT_NO_ALPHA for no transparency)
              gbuffer_t src,    // pointer to source buffer
              gbuffer_t dst) {  // pointer to destination buffer
// This is the fractional part of a number when expressing a float as a fixed point integer.
#define UNIT_LSB 16
  uint16_t width = gbuf_get_width(src);
  uint16_t height = gbuf_get_height(src);
  // WA for a bug that log2 returns e.g. 6.99 instead of 7.00
  // which then get rounded down...
  uint16_t width_log = round(log2(width));
  uint16_t height_log = round(log2(height));

  // shift and mask need to take color depth into account.
  // Because if color depth is two bytes per pixel then 
  // the image data will be twice as large  
  // "1" in log2 means: * 2
  #if LCD_COLORDEPTH == 16
  #define INTERP_COL_DEPTH 1
  #elif LCD_COLORDEPTH == 8
  #define INTERP_COL_DEPTH 0
  #endif

  interp_config lane0_cfg = interp_default_config();
  // The shift is for the fixed point integer <-> float reresentation. 16 bits, so 65536 represent 1
  interp_config_set_shift(&lane0_cfg, UNIT_LSB - INTERP_COL_DEPTH);
  // the masking is so that you don't run out of the image's line area
  interp_config_set_mask(&lane0_cfg, INTERP_COL_DEPTH, INTERP_COL_DEPTH + (width_log - 1));
  interp_config_set_add_raw(&lane0_cfg, true);  // Add full accumulator to base with each POP
  interp_config lane1_cfg = interp_default_config();
  interp_config_set_shift(&lane1_cfg, UNIT_LSB - (INTERP_COL_DEPTH + width_log));
  // the masking is so that you don't run out of the image's area: width_log + height_log in log2 means: width_log * height_log
  interp_config_set_mask(&lane1_cfg, INTERP_COL_DEPTH + width_log, INTERP_COL_DEPTH + (width_log + height_log - 1));
  interp_config_set_add_raw(&lane1_cfg, true);

  interp_set_config(interp0, 0, &lane0_cfg);
  interp_set_config(interp0, 1, &lane1_cfg);
  interp0->base[2] = (uint32_t)src.data;

  float zoom_x = zoom;  //shearing x
  float zoom_y = zoom;  //shearing y

  int32_t rotate[4] = {
    (int32_t)(cosf(rot) / zoom_x * (1 << UNIT_LSB)), (int32_t)(-sinf(rot) / zoom_y * (1 << UNIT_LSB)),
    (int32_t)(sinf(rot) / zoom_x * (1 << UNIT_LSB)), (int32_t)(cosf(rot) / zoom_y * (1 << UNIT_LSB))
  };

  interp0->base[0] = rotate[0];
  interp0->base[1] = rotate[2];

  // Groesse des sichtbaren Feldes im FB
  uint16_t vis_size = width > height ? width * zoom_x / sqrt(2) / sqrt(1.75f) : height * zoom_y / sqrt(2) / sqrt(1.75f);

  // px/y: Translations innerhalb des Arrays im Framebuffer
  int px = vis_size / 2;
  int py = -vis_size / 2;

  // pivot_x/y: Koordinaten innerhalb des Drehpunkts innerhalb der Quelle
  int pivot_x = width / 2;
  int pivot_y = height / 2;

  // No translation i.e. kx and ky are the upper left coords for blitting into the fb
  //kx += width * zoom_x / sqrt(2.0f) / sqrt(1.75f) / 2;
  //ky += height * zoom_y / sqrt(2.0f) / sqrt(1.75f) / 2;

  // out-of-framebuffer checks
  int start_y, end_y, start_x, end_x;
  int shift_x = 0;

  if (ky + vis_size / 2 > gbuf_get_height(dst))
    end_y = gbuf_get_height(dst);
  else
    end_y = ky + vis_size / 2;

  if (ky - vis_size / 2 < 0)
    start_y = 0;
  else
    start_y = ky - vis_size / 2;

  if (kx + vis_size / 2 > gbuf_get_width(dst))
    end_x = gbuf_get_width(dst);
  else
    end_x = kx + vis_size / 2;

  if (kx - vis_size / 2 < 0) {
    start_x = 0;
    shift_x = -(kx - (vis_size / 2));
  } else
    start_x = kx - vis_size / 2;

  // shift_x is for shifting in case of pushing out of the left buffer edge so the loop needs to start at some later point.
  // So we have to start the accum reg with a higher value. (shift_x * base[0/1])
  int32_t accum0_start = rotate[1] * (-ky + vis_size / 2 + py) - rotate[0] * px + pivot_x * (1 << UNIT_LSB) + shift_x * rotate[0];
  int32_t accum1_start = rotate[3] * (-ky + vis_size / 2 + py) - rotate[2] * px + pivot_y * (1 << UNIT_LSB) + shift_x * rotate[2];

  uint16_t dst_width = gbuf_get_width(dst);

  for (int y = start_y; y < end_y; ++y) {
    interp0->accum[0] = rotate[1] * y + accum0_start;
    interp0->accum[1] = rotate[3] * y + accum1_start;
    for (int x = start_x; x < end_x; ++x) {
      color_t colour = *(color_t *)(interp0->pop[2]);
      if (colour != alpha)
        dst.data[x + y * dst_width] = colour;
    }
  }
}  // blitRotZoomBuf


/*
 * Blits a source buffer to a destination buffer
 * while flipping and/or zooming it.
 * alpha must equal either BLIT_NO_ALPHA (no alpha channel) or the
 * color that shall be transparent
 * Both width and height must be a power of 2
 */
void blit_buf(coord_t kx,       // x-coord where to blit the of CENTER of the image
              coord_t ky,       // y-coord where to blit the of CENTER of the image
              float zoom_x,     // zoom factor in x direction
              float zoom_y,     // zoom factor in y direction
              uint8_t flip,     // whether to flip the image
              int32_t alpha,    // color which is NOT being drawn (BLIT_NO_ALPHA for no transparency)
              gbuffer_t src,    // pointer to source buffer
              gbuffer_t dst) {  // pointer to destination buffer

// This is the fractional part of a number when expressing a float as a fixed point integer.
#define UNIT_LSB 16
  uint16_t width = gbuf_get_width(src);
  uint16_t height = gbuf_get_height(src);
  // WA for a bug that log2 returns e.g. 6.99 instead of 7.00
  // which then get rounded down...
  uint16_t width_log = round(log2(width));
  uint16_t height_log = round(log2(height));

  // shift and mask need to take color depth into account.
  // Because if color depth is two bytes per pixel then
  // the image data will be twice as large
  // "1" in log2 means: * 2
  #if LCD_COLORDEPTH == 16
  #define INTERP_COL_DEPTH 1
  #elif LCD_COLORDEPTH == 8
  #define INTERP_COL_DEPTH 0
  #endif

  // Can probably be done with only one lane. Revise.
  interp_config lane0_cfg = interp_default_config();
  // The shift is for the fixed point integer <-> float reresentation. 16 bits, so 65536 represent 1
  interp_config_set_shift(&lane0_cfg, UNIT_LSB - INTERP_COL_DEPTH);
  // the masking is so that you don't run out of the image's line area
  interp_config_set_mask(&lane0_cfg, INTERP_COL_DEPTH, INTERP_COL_DEPTH + (width_log - 1));
  interp_config_set_add_raw(&lane0_cfg, true);  // Add full accumulator to base with each POP
  interp_config lane1_cfg = interp_default_config();
  interp_config_set_shift(&lane1_cfg, UNIT_LSB - (INTERP_COL_DEPTH + width_log));
  // the masking is so that you don't run out of the image's area: width_log + height_log in log2 means: width_log * height_log
  interp_config_set_mask(&lane1_cfg, INTERP_COL_DEPTH + width_log, INTERP_COL_DEPTH + (width_log + height_log - 1));
  interp_config_set_add_raw(&lane1_cfg, true);

  interp_set_config(interp0, 0, &lane0_cfg);
  interp_set_config(interp0, 1, &lane1_cfg);
  interp0->base[2] = (uint32_t)src.data;

  int32_t zoom[2] = {
    (int32_t)(1.0f / zoom_x * (1 << UNIT_LSB)),
    (int32_t)(1.0f / zoom_y * (1 << UNIT_LSB))
  };

  interp0->base[0] = zoom[0];
  interp0->base[1] = 0;

  // pivot_x/y: Koordinaten innerhalb des Drehpunkts innerhalb der Quelle
  int16_t pivot_x = +(width / 2 - 1);
  int16_t pivot_y = -(height / 2 - 1);

  // px/y: Translations innerhalb des Arrays im Framebuffer
  int16_t px = (int)+(width * zoom_x / 2 + 1);
  int16_t py = (int)-(height * zoom_y / 2 + 1);

  // rx/y: Groesse des sichtbaren Feldes im FB
  int ry = (int)height * zoom_y;
  int rx = (int)width * zoom_x;

  // out-of-framebuffer checks
  int start_y, end_y, start_x, end_x;
  int shift_x = 0;

  if (ky + ry / 2 > gbuf_get_height(dst))
    end_y = gbuf_get_height(dst);
  else
    end_y = ky + ry / 2;

  if (ky - ry / 2 < 0)
    start_y = 0;
  else
    start_y = ky - ry / 2;

  if (kx + rx / 2 > gbuf_get_width(dst))
    end_x = gbuf_get_width(dst);
  else
    end_x = kx + rx / 2;

  if (kx - rx / 2 < 0) {
    start_x = 0;
    shift_x = -(kx - (rx / 2));
  } else
    start_x = kx - rx / 2;

  // shift_x is for shifting the visible part of the image to the right
  // when pushing out of the left boarder so the loop needs to start at
  // some later point, so we have to start the accum reg with a higher
  // value. (shift_x * base[0/1])
  int32_t accum0_start = -zoom[0] * px + pivot_x * (1 << UNIT_LSB) + shift_x * zoom[0];
  int32_t accum1_start = zoom[1] * (-ky + ry / 2 + py) + pivot_y * (1 << UNIT_LSB);

  uint32_t dst_width = gbuf_get_width(dst);

  for (int y = start_y; y < end_y; ++y) {
    interp0->accum[0] = accum0_start;
    if (flip && BLIT_FLIP_VERT)
      interp0->accum[1] = zoom[1] * (end_y + start_y - y - 1) + accum1_start;
    else
      interp0->accum[1] = zoom[1] * y + accum1_start;

    if (flip && BLIT_FLIP_HORI) {
      for (int x = end_x - 1; x >= start_x; x--) {
        color_t colour = *(color_t *)(interp0->pop[2]);
        if (colour != alpha)
          dst.data[x + y * dst_width] = colour;
      }
    } else {
      for (int x = start_x; x < end_x; ++x) {
        color_t colour = *(color_t *)(interp0->pop[2]);
        if (colour != alpha)
          dst.data[x + y * dst_width] = colour;
      }
    }
  }
}  // blit_buf
