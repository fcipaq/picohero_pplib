/*
 * pplib - a library for the Pico Held handheld
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
 
#ifndef PRIMITIVES_H
#define PRIMITIVES_H

/* ========================== includes ========================== */
#include "../typedefs.h"

/* ====================== function declarations ====================== */
// Drawing primitives
void draw_pixel(coord_t x, coord_t y, color_t color, gbuffer_t dst);
void draw_rect_fill(coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color, gbuffer_t dst);
void draw_rect(coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color, gbuffer_t dst);
void draw_line(coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color, gbuffer_t dst);
void draw_line_interp(coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color, gbuffer_t dst);

#define swap_coords(x, y) {coord_t temporary_swap_coordinate = x; x = y; y = temporary_swap_coordinate;}
#define check_coord(x, y) {if (x > y) {swapCoord(x, y)}}

#endif // PRIMITIVES_H
