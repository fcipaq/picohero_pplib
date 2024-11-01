/*
 * pplib - a library for the Pico Held handheld
 *
 * Copyright (C) 2023 Daniel Kammer (daniel.kammer@web.de)
 *
 * blit_tile_map_mode7 is based on boochow's pico_test_projects/tree/main/interpolator2-vga
 * released under MIT liscense (https://github.com/boochow/pico_test_projects/)
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

#if 1

#include "gbuffers.h"
#include "blitter.h"
#include "hardware/interp.h"

//#include <inttypes.h>
#include <math.h>

// This is the fractional part of a number when expressing a float as a fixed point integer. 
#define BITS_FRACT 16
  
	
void tile_blit_rot(coord_t kx,            // start in fb window x
                   coord_t ky,            // start in fb window y
                   coord_t w,             // window width
                   coord_t h,             // window height
                   coord_t px,            // translation within window
                   coord_t py,  
                   coord_t pivot_x,       // pivot point (in screen/buffer coords, 0/0 is upper left corner)
                   coord_t pivot_y,  
                   float rot,             // angle
                   float zoom_x,          // zoom in horizontal direction
                   float zoom_y,          // zoom in vertical direction
                   tile_map_t map_data,   // map data
                   tile_data_t tile_set,  // tile data
                   color_t alpha,         // transparency
                   gbuffer_t buf) {       // pointer to destination buffer

  // log2 may return e.g. 6.99 instead of 7.00 which needs to get rounded up
  uint16_t map_width_log = round(log2(map_data.width));
  uint16_t map_height_log = round(log2(map_data.height));

  uint16_t tiles_width = tile_set.width;
  uint16_t tiles_height = tile_set.height;

  uint16_t tiles_width_log = round(log2(tiles_width));
  uint16_t tiles_height_log = round(log2(tiles_height));

  uint16_t buf_width = gbuf_get_width(buf);
  uint16_t buf_height = gbuf_get_height(buf);

  interp_config lane0_cfg = interp_default_config();
  
  interp_config_set_shift(&lane0_cfg, BITS_FRACT);                 // The shift is for the fixed point integer <-> float reresentation. 16 bits, so 65536 represent 1
  interp_config_set_mask(&lane0_cfg, 0, map_width_log - 1);      // the masking is so that you don't run out of the image's line area 
  interp_config_set_add_raw(&lane0_cfg, true);                   // Add full accumulator to base with each POP
  interp_config lane1_cfg = interp_default_config();
  interp_config_set_shift(&lane1_cfg, BITS_FRACT - map_width_log); // the masking is so that you don't run out of the image's area: width_log + height_log in log2 means: width_log * height_log 
  interp_config_set_mask(&lane1_cfg,  map_width_log,  map_width_log + map_height_log - 1);
  interp_config_set_add_raw(&lane1_cfg, true);

  interp_set_config(interp0, 0, &lane0_cfg);
  interp_set_config(interp0, 1, &lane1_cfg);
  
  interp0->base[2] = (uint32_t) map_data.data;

  lane0_cfg = interp_default_config();
  interp_config_set_shift(&lane0_cfg, BITS_FRACT);
  interp_config_set_mask(&lane0_cfg, 0, tiles_width_log - 1);
  interp_config_set_add_raw(&lane0_cfg, true);
  lane1_cfg = interp_default_config();
  interp_config_set_shift(&lane1_cfg, BITS_FRACT - tiles_width_log);
  interp_config_set_mask(&lane1_cfg, tiles_width_log, tiles_width_log + tiles_height_log - 1);
  interp_config_set_add_raw(&lane1_cfg, true);

  interp_set_config(interp1, 0, &lane0_cfg);
  interp_set_config(interp1, 1, &lane1_cfg);
  
  interp1->base[2] = 0;
  
  zoom_x *= tiles_width;
  zoom_y *= tiles_height;

  int32_t rotate[4] = {
    (int32_t) (cosf(rot) / zoom_x * (1 << BITS_FRACT)), (int32_t) (-sinf(rot) / zoom_y * (1 << BITS_FRACT)),
    (int32_t) (sinf(rot) / zoom_x * (1 << BITS_FRACT)), (int32_t) (cosf(rot) / zoom_y * (1 << BITS_FRACT))
  };

  // out-of-framebuffer checks
  int start_y, end_y, start_x, end_x;
  int shift_x = 0;
  
  if (ky + h > buf_height)
    end_y = buf_height;
  else
    end_y = ky + h;
    
  if (ky < 0)
    start_y = 0;
  else
    start_y = ky;

  if (kx + w > buf_width)
    end_x = buf_width;
  else
    end_x = kx + w;

  if (kx < 0) {
    start_x = 0;
    shift_x = -kx;  
  } else
    start_x = kx;
  
  int32_t accum0_start =  rotate[1] * ( - ky - pivot_y) + rotate[0] * (shift_x - pivot_x) + px * (1 << BITS_FRACT - tiles_width_log);
  int32_t accum1_start =  rotate[3] * ( - ky - pivot_y) + rotate[2] * (shift_x - pivot_x) + py * (1 << BITS_FRACT - tiles_height_log); 

  interp0->base[0] = rotate[0];
  interp0->base[1] = rotate[2];
  interp1->base[0] = rotate[0] * tiles_width;
  interp1->base[1] = rotate[2] * tiles_height;

  uint32_t tiles_ofs = tiles_width * tiles_height;

  if (alpha == BLIT_NO_ALPHA) {
    for (int y = start_y ; y < end_y; ++y) {
      interp0->accum[0] = rotate[1] * y + accum0_start;
      interp0->accum[1] = rotate[3] * y + accum1_start;
      interp1->accum[0] = (rotate[1] * y + accum0_start) * tiles_width;
       interp1->accum[1] = (rotate[3] * y + accum1_start) * tiles_height;

        for (int x = start_x; x < end_x; ++x) {
            uint8_t t = *(uint8_t *) interp0->pop[2];
            uint16_t c = (uint16_t) interp1->pop[2];
            buf.data[x + y * buf_width] = tile_set.image->data[t * tiles_ofs + c];
	    }
    }
  } else {
    for (int y = start_y ; y < end_y; ++y) {
      interp0->accum[0] = rotate[1] * y + accum0_start;
      interp0->accum[1] = rotate[3] * y + accum1_start;
      interp1->accum[0] = (rotate[1] * y + accum0_start) * tiles_width;
      interp1->accum[1] = (rotate[3] * y + accum1_start) * tiles_height;

      for (int x = start_x; x < end_x; ++x) {
          uint8_t t = *(uint8_t *) interp0->pop[2];
          uint32_t c = (uint32_t) interp1->pop[2];
          color_t col = tile_set.image->data[t * tiles_ofs + c];
		  if (col != alpha)
			buf.data[x + y * buf_width] = col;
	  }
    }
  }

}  // tile_blit_rot


void tile_blit_mode7(coord_t kx,           // start in fb window x
                     coord_t ky,           // start in fb window y
                     coord_t w,            // window width
                     coord_t h,            // window height
                     float px,             // translation within window
                     float py,             // translation within window
                     float pz,             // translation within window
                     float pr,             // rotation within window
                     tile_map_t map_data,  // map data
                     tile_data_t tile_set, // tile data
                     color_t alpha,         // transparency
                     gbuffer_t buf) {      // pointer to destination buffer

  uint16_t map_width = map_data.width;
  uint16_t map_height = map_data.height;

  // log2 may return e.g. 6.99 instead of 7.00 which needs to get rounded up
  uint16_t map_width_log = round(log2(map_width));
  uint16_t map_height_log = round(log2(map_height));

  uint16_t tiles_width = tile_set.width;
  uint16_t tiles_height = tile_set.height;

  // coordinate sanity check and conversion
  if ((px < 0) || (py < 0) || (px > map_width * tiles_width) || (py > map_height * tiles_height))
    return;

  uint16_t tiles_width_log = round(log2(tiles_width));
  uint16_t tiles_height_log = round(log2(tiles_height));

  interp_config lane0_cfg = interp_default_config();  // The shift is for the fixed point integer <-> float reresentation. 16 bits, so 65536 represent 1
  interp_config_set_shift(&lane0_cfg, BITS_FRACT);  // the masking is so that you don't run out of the image's line area 
  interp_config_set_mask(&lane0_cfg, 0, map_width_log - 1);
  interp_config_set_add_raw(&lane0_cfg, true);  // Add full accumulator to base with each POP
  interp_config lane1_cfg = interp_default_config();
  interp_config_set_shift(&lane1_cfg, BITS_FRACT - map_width_log); // the masking is so that you don't run out of the image's area: width_log + height_log in log2 means: width_log * height_log 
  interp_config_set_mask(&lane1_cfg,  map_width_log,  map_width_log + map_height_log - 1);
  interp_config_set_add_raw(&lane1_cfg, true);

  interp_set_config(interp0, 0, &lane0_cfg);
  interp_set_config(interp0, 1, &lane1_cfg);
  
  interp0->base[2] = (uint32_t) map_data.data;

  lane0_cfg = interp_default_config();
  interp_config_set_shift(&lane0_cfg, BITS_FRACT);
  interp_config_set_mask(&lane0_cfg, 0, tiles_width_log - 1);
  interp_config_set_add_raw(&lane0_cfg, true);
  lane1_cfg = interp_default_config();
  interp_config_set_shift(&lane1_cfg, BITS_FRACT - tiles_width_log);
  interp_config_set_mask(&lane1_cfg, tiles_width_log, tiles_width_log + tiles_height_log - 1);
  interp_config_set_add_raw(&lane1_cfg, true);

  interp_set_config(interp1, 0, &lane0_cfg);
  interp_set_config(interp1, 1, &lane1_cfg);
  
  interp1->base[2] = 0;
  
  // out-of-framebuffer checks
  int start_y, end_y, start_x, end_x;
  int shift_x = 0;
  
  if (ky + h > gbuf_get_height(buf))
    end_y = gbuf_get_height(buf);
  else
    end_y = ky + h;
    
  if (ky < 0)
    start_y = 0;
  else
    start_y = ky;

  if (kx + w > gbuf_get_width(buf))
    end_x = gbuf_get_width(buf);
  else
    end_x = kx + w;

  if (kx < 0) {
    start_x = 0;
    shift_x = -kx;  
  } else
    start_x = kx;
	
  float rcos = cosf(pr);
  float rsin = sinf(pr);

  uint32_t tiles_ofs = tiles_width * tiles_height;
  
  // Are the px and py positions dependent on pz (which is bad)?
  // TODO: revise
  uint32_t px32 = px * (1 << (BITS_FRACT - tiles_width_log));  // same as / tiles_width
  uint32_t py32 = py * (1 << (BITS_FRACT - tiles_height_log)); // same as / tiles_height
  uint32_t pz32 = (float) pz / 100. * (1 << BITS_FRACT);
 
  uint16_t fb_width = gbuf_get_width(buf);
 
  for (int y = start_y ; y < end_y ; ++y) {
    int32_t n = pz32 / (y - ky + 1);
    int32_t s = 400 * n;  //TODO distortion
    int32_t t2 = w * n;
    int32_t u = px32 +   t2 / 2  * rcos + s * rsin;
    int32_t v = py32 + (-t2 / 2) * rsin + s * rcos;
    int32_t du = -rcos * n;
    int32_t dv = rsin * n;

    // TODO: revise breaking on large s
	if (s > (200 << BITS_FRACT)) {
      continue;
    }
	
    // shift_x is for shifting the visible part of the image to the right
    // when pushing out of the left screen boarder so the loop needs to
	// start at some later point, so the accum reg needs to start with a
	// higher value. (shift_x * base[0/1])
    interp0->accum[0] = u + shift_x * du;
    interp0->base[0] = du;
    interp0->accum[1] = v + shift_x * dv;
    interp0->base[1] = dv;
	
    interp1->accum[0] = (u + shift_x * du) * tiles_width;
    interp1->base[0] = du * tiles_width;
    interp1->accum[1] = (v + shift_x * dv) * tiles_height;
    interp1->base[1] = dv * tiles_height;

    if (alpha == BLIT_NO_ALPHA) {  
	  for (int x = start_x; x < end_x; ++x) {
          uint8_t t = *(uint8_t *) interp0->pop[2];
          uint16_t c = (uint16_t) interp1->pop[2];
          buf.data[x + y * fb_width] = tile_set.image->data[t * tiles_ofs + c];
	  }
	} else {
	  for (int x = start_x; x < end_x; ++x) {
          uint8_t t = *(uint8_t *) interp0->pop[2];
          uint16_t c = (uint16_t) interp1->pop[2];
          color_t col = tile_set.image->data[t * tiles_ofs + c];
		  if (col != alpha)
            buf.data[x + y * fb_width] = col;
	  }
	}
  }

}  // tile_blit_mode7


#endif  //#ifdef ARDUINO_ARCH_RP2040