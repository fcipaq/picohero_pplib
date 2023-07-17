/*
 * pplib - a library for the PicoPal handheld
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

#include "primitives.h"
#include "gbuffers.h"
#include "hardware/interp.h"

void draw_pixel(coord_t x, coord_t y, color_t color, gbuffer_t dst) {

  if (x >= 0 && x < gbuf_get_width(dst) && y >= 0 && y < gbuf_get_height(dst))
    dst.data[y * gbuf_get_width(dst) + x] = color;
}

//void __scratch_x("DrawLine") DrawLine(coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color, gbuffer_t dst) {
void draw_line(coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color, gbuffer_t dst) {

  uint16_t bufwidth = gbuf_get_width(dst);
  uint16_t bufheight = gbuf_get_height(dst);
  uint32_t bufsize = bufwidth * bufheight;

  int16_t x = x1, y = y1, dx, dy, incx, incy, err;

  if (x2 >= x1) {
    dx = x2 - x1;
    incx = 1;
  } else {
    dx = x1 - x2;
    incx = -1;
  }

  if (y2 >= y1) {
    dy = y2 - y1;
    incy = 1;
  } else {
    dy = y1 - y2;
    incy = -1;
  }

  if (x1 < bufwidth && x1 >= 0 && x2 < bufwidth && x2 >= 0 && y1 < bufheight && y1 >= 0 && y2 < bufheight && y2 >= 0) {
    /* ------------ all points within viewport ------------ */

    // horizontal lines cannot be drawn by loop
    if (y1 == y2) {
      if (x1 < x2) {
        for (uint32_t h = x1 + y1 * bufwidth; h < x2 + 1 + y1 * bufwidth; h++)
          dst.data[h] = color;
      } else {
        for (uint32_t h = x2 + y1 * bufwidth; h < x1 + 1 + y1 * bufwidth; h++)
          dst.data[h] = color;
      }
      return;
    }

    incy *= bufwidth;  // direct pointer address calculation

    color_t *target_p = &dst.data[y2 * bufwidth + x2];
    color_t *buf_ptr = &dst.data[y * bufwidth + x];

    if (dx >= dy) {
      dy <<= 1;
      err = dy - dx;
      dx <<= 1;
      while (buf_ptr != target_p) {
        *buf_ptr = color;
        if (err >= 0) {
          buf_ptr += incy;
          err -= dx;
        }
        err += dy;
        buf_ptr += incx;
      }
    } else {
      // repeated, because that means one if less in the loop --> faster
      dx <<= 1;
      err = dx - dy;
      dy <<= 1;
      while (buf_ptr != target_p) {
        *buf_ptr = color;
        if (err >= 0) {
          buf_ptr += incx;
          err -= dy;
        }
        err += dx;
        buf_ptr += incy;
      }
    }

  } else [[unlikely]] {
    /* ------------ at least one point out of viewport ------------ */

    // horizontal lines cannot be drawn by loop
    if (y1 == y2) {
      if (x1 < x2) {
        for (uint32_t h = x1 + y1 * bufwidth; h < x2 + 1 + y1 * bufwidth; h++)
          if (h >= 0 && h < bufsize && ((h - bufwidth * y) < bufwidth) && ((h - bufwidth * y) >= 0))
            dst.data[h] = color;
      } else {
        for (uint32_t h = x2 + y1 * bufwidth; h < x1 + 1 + y1 * bufwidth; h++)
          if (h >= 0 && h < bufsize && ((h - bufwidth * y) < bufwidth) && ((h - bufwidth * y) >= 0))
            dst.data[h] = color;
      }
      return;
    }

    if (dx >= dy) {
      // shl is faster but integer might be signed
      dy *= 2;
      err = dy - dx;
      dx *= 2;
      while (x != x2) {
        if (x >= 0 && x < bufwidth && y >= 0 && y < bufheight)
          dst.data[y * bufwidth + x] = color;
        if (err >= 0) {
          y += incy;
          err -= dx;
        }
        err += dy;
        x += incx;
      }
    } else {
      // repeated, because that means one "if" less in the loop --> faster
      // shl is faster but integer might be signed
      dx *= 2;
      err = dx - dy;
      dy *= 2;
      while (y != y2) {
        if (x >= 0 && x < bufwidth && y >= 0 && y < bufheight)
          dst.data[y * bufwidth + x] = color;
        if (err >= 0) {
          x += incx;
          err -= dy;
        }
        err += dx;
        y += incy;
      }
    }
  }

}  // DrawLine

#if 0
void draw_line_interp(coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color, gbuffer_t dst) {
  uint16_t width = gbuf_get_width(dst);
  uint16_t height = gbuf_get_height(dst);

  float m = ((float)y1 - (float)y2) / ((float)x1 - (float)x2);

  if (x1 < 0) {
    y1 -= m * x1;
    x1 = 0;
  }

  if (x2 < 0) {
    y2 -= m * x2;
    x2 = 0;
  }

  if (y1 < 0) {
    x1 -= (int)((float)y1 / m);
    y1 = 0;
  }

  if (y2 < 0) {
    x2 -= (int)((float)y2 / m);
    y2 = 0;
  }

  if (x1 >= width) {
    y1 -= m * (x1 - width + 1);
    x1 = width - 1;
  }

  if (x2 >= width) {
    y2 -= m * (x2 - width + 1);
    x2 = width - 1;
  }

  if (y1 >= height) {
    x1 -= (int)((float)(y1 - height + 1) / m);
    y1 = height - 1;
  }

  if (y2 >= height) {
    x2 -= (int)((float)(y2 - height + 1) / m);
    y2 = height - 1;
  }

  if (!((x1 >= 0) && (x1 < width) && (y1 >= 0) && (y1 <= height) && (x2 >= 0) && (x2 < width) && (y2 >= 0) && (y2 <= height)))
    return;

  #define BITS_FRACT 14  // 18 bits = 131072 bytes max --> max. 480x320 framebuffer
  interp_config lane0_cfg = interp_default_config();
  interp_config_set_shift(&lane0_cfg, BITS_FRACT);
  interp_config_set_add_raw(&lane0_cfg, true);

  interp_config lane1_cfg = interp_default_config();

  interp_set_config(interp0, 0, &lane0_cfg);
  interp_set_config(interp0, 1, &lane1_cfg);

  interp0->accum[1] = 0;
  interp0->base[1] = 0;

  if (abs(x2 - x1) > abs(y2 - y1)) {
    interp0->base[0] = abs(((y2 - y1) * (1 << BITS_FRACT)) / (x2 - x1));

    if (x1 > x2) {
      coord_t tmp = x1;
      x1 = x2;
      x2 = tmp;

      tmp = y1;
      y1 = y2;
      y2 = tmp;
    }

    interp0->accum[0] = 0;

    if (m >= 0) {
      interp0->base[2] = y1;
      for (int x = x1; x < x2 + 1; x++)
        dst.data[x + interp0->pop[2] * width] = color;
    } else {
      interp0->base[2] = y2;
      for (int x = x2; x > x1 - 1; x--)
        dst.data[x + interp0->pop[2] * width] = color;
    }

  } else {
    interp0->base[2] = 0;

    if (y1 > y2) {
      coord_t tmp = x1;
      x1 = x2;
      x2 = tmp;

      tmp = y1;
      y1 = y2;
      y2 = tmp;
    }

    if (m >= 0) {
      interp0->base[0] = abs(((x2 - x1) * (1 << BITS_FRACT)) / (y2 - y1)) + width * (1 << BITS_FRACT);
      interp0->accum[0] = (x1 + y1 * width) * (1 << BITS_FRACT);

      for (int y = y1; y < y2 + 1; y++)
        dst.data[interp0->pop[2]] = color;

    } else {
      interp0->base[0] = abs(((x2 - x1) * (1 << BITS_FRACT)) / (y2 - y1));
      interp0->accum[0] = (x2 + y2 * width) * (1 << BITS_FRACT);

      uint32_t y_diff = 0;

      for (int y = y1; y < y2 + 1; y++) {
        dst.data[interp0->pop[2] - y_diff] = color;
        y_diff += width;
      }
    }
  }

}  // DrawLineInterp
#endif

void sanitize_rect(coord_t *x1, coord_t *y1, coord_t *x2, coord_t *y2, gbuffer_t dst) {
  uint16_t width = gbuf_get_width(dst) - 1;
  uint16_t height = gbuf_get_height(dst) - 1;
  
  // range checking
  if (*x1 < 0)
    *x1 = 0;

  if (*x1 > width)
    *x1 = width;

  if (*x2 < 0)
    *x2 = 0;

  if (*x2 > width)
    *x2 = width;

  if (*y1 < 0)
    *y1 = 0;

  if (*y1 > height)
    *y1 = height;

  if (*y2 < 0)
    *y2 = 0;

  if (*y2 > height)
    *y2 = height;

  if (*x1 > *x2) {
    coord_t tmp;
    tmp = *x1;
    *x1 = *x2;
    *x2 = tmp;
  }

  if (*y1 > *y2) {
    coord_t tmp;
    tmp = *y1;
    *y1 = *y2;
    *y2 = tmp;
  }
}

void draw_rect_fill(coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color, gbuffer_t dst) {
  sanitize_rect(&x1, &y1, &x2, &y2, dst);

  uint16_t rect_width = x2 - x1 + 1;
  uint16_t rect_height = y2 - y1 + 1;

  uint16_t buf_width = gbuf_get_width(dst);

  //jumpy  off by one?!
  for (uint16_t y = y1; y < (y2 + 1); y++)
    for (uint16_t x = x1; x < (x2 + 1); x++)
      dst.data[y * buf_width + x] = color;
}

void draw_rect(coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color, gbuffer_t dst) {
  sanitize_rect(&x1, &y1, &x2, &y2, dst);

  uint16_t rect_width = x2 - x1;
  uint16_t rect_height = y2 - y1;
  
  if ((rect_width == 0) || (rect_height == 0))
	return;

  uint16_t buf_width = gbuf_get_width(dst);
  
  uint32_t y1_ofs = y1 * buf_width;
  uint32_t y2_ofs = y2 * buf_width;
  
  for (uint16_t x = x1; x < (x2 + 1); x++) {
    dst.data[y1_ofs + x] = color;
    dst.data[y2_ofs + x] = color;
  }

  for (uint16_t y = y1; y < (y2 + 1); y++) {
	uint32_t y_ofs = y * buf_width;
    dst.data[y_ofs + x1] = color;
    dst.data[y_ofs + x2] = color;
  }
  
}
