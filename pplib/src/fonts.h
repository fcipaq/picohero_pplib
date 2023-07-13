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

#ifndef FONTS_H
#define FONTS_H

/* ========================== includes ========================== */
#include "typedefs.h"

/* ==================== function declarations =================== */
// measurement
int  font_get_width(font_t* font);
int  font_get_height(font_t* font);
int  font_get_char_width(char c, font_t* font);
int  font_get_string_width(char* str, font_t* font);

// output
void font_put_char(coord_t pos_x, coord_t pos_y, color_t col, char c, font_t* font, gbuffer_t dst);
void font_write_string(coord_t pos_x, coord_t pos_y, color_t col, char* str, font_t* font, gbuffer_t dst);
void font_write_string_centered(coord_t pos_x, coord_t y, color_t col, char* str, font_t* font, gbuffer_t dst);


#endif //FONTS_H
