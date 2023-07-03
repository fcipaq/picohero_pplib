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

#ifndef BLITTER_H
#define BLITTER_H

/* ========================== includes ========================== */
#include "typedefs.h"

/* ======================== definitions ========================= */
#define BLIT_NO_ALPHA -1

/* ====================== function declarations ====================== */
void blit_buf(coord_t kx,     // coordinates of upper left corner
              coord_t ky,
              gbuffer_t src,  // pointer to source buffer
              gbuffer_t dst,  // pointer to destination buffer
              int32_t alpha);

void blit_buf(coord_t kx,       // x-coord where to blit the of CENTER of the image
              coord_t ky,       // y-coord where to blit the of CENTER of the image
              float zoom,       // zoom factor (same in both directions)
              float rot,        // rotation of the image (in rad)
              int32_t alpha,    // color which is NOT being drawn (BLIT_NO_ALPHA for no transparency)
              gbuffer_t src,    // pointer to source buffer
              gbuffer_t dst);   // pointer to destination buffer

void blit_buf(coord_t kx,       // x-coord where to blit the of CENTER of the image
              coord_t ky,       // y-coord where to blit the of CENTER of the image
              float zoom_x,     // zoom factor in x direction
              float zoom_y,     // zoom factor in y direction
              uint8_t flip_x,   // whether to flip horizontally (1 means flip, 0 means
              uint8_t flip_y,   // whether to flip vertically
              int32_t alpha,    // color which is NOT being drawn (BLIT_NO_ALPHA for no transparency)
              gbuffer_t src,    // pointer to source buffer
              gbuffer_t dst);   // pointer to destination buffer
#endif // BLITTER_H
