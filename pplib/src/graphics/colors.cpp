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

#pragma GCC optimize("Ofast")

#include "colors.h"

color8_t rgb_col_332_332(uint8_t r, uint8_t g, uint8_t b)
{
  return (r * 32 + g * 4 + b);
}

color8_t rgb_col_888_332(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xE0) | ((g & 0xE0) >> 3) | ((b & 0xC0) >> 6));
}

uint8_t rgb_col_565_red(color16_t col) {
  return (col >> 11) & 0x1f;
}

uint8_t rgb_col_565_green(color16_t col) {
  return (col >> 5) & 0x3f;
}

uint8_t rgb_col_565_blue(color16_t col) {
  return (col & 0x1f);
}

color16_t rgb_col_565_565(uint8_t r, uint8_t g, uint8_t b)
{
  return (r << 11 | g << 5 | b);
}

color16_t rgb_col_888_565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}