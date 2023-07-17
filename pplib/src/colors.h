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

#ifndef COLORS_H
#define COLORS_H

/* ========================== includes ========================== */
#include "typedefs.h"

/* ==================== function declarations =================== */
color16_t rgb_col_888_565(uint8_t r, uint8_t g, uint8_t b);
color8_t  rgb_col_888_332(uint8_t r, uint8_t g, uint8_t b);
color16_t rgb_col_565_565(uint8_t r, uint8_t g, uint8_t b);
color8_t  rgb_col_332_332(uint8_t r, uint8_t g, uint8_t b);
uint8_t   rgb_col_565_red(color16_t col);
uint8_t   rgb_col_565_green(color16_t col);
uint8_t   rgb_col_565_blue(color16_t col);

#endif // COLORS_H
