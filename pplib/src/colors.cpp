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

#include "colors.h"

/*
 * Returns the color in 8 bit machine readable format when given
 * r g and b values in 8 bits. R-G-B 3-3-2
 * Ranges from r: 0..7, g: 0..7 and b: 0..3
 */
color8_t RGBColor332_332(uint8_t r, uint8_t g, uint8_t b)
{
  return (r * 32 + g * 4 + b);
}

/*
   Returns the color in 8 bit in machine readable format when given
   r g and b values in 24 bits. R-G-B 8-8-8
   So r ranges from 0..8, g: 0..8 and b: 0..3
*/
color8_t RGBColor888_332(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xE0) | ((g & 0xE0) >> 3) | ((b & 0xC0) >> 6));
}

/*
 * Returns the red component of a RGB565 color as a value ranging from 0..31
 */
uint8_t RGB565_red5(color16_t col) {
  return (col >> 11) & 0x1f;
}

/*
 * Returns the green component of a RGB565 color as a value ranging from 0..63
 */
uint8_t RGB565_green6(color16_t col) {
  return (col >> 5) & 0x3f;
}

/*
 * Returns the blue component of a RGB565 color as a value ranging from 0..31
 */
uint8_t RGB565_blue5(color16_t col) {
  return (col & 0x1f);
}

/*
   Returns the color in 16 bit machine readable format when given
   r g and b values in 16 bits. R-G-B 5-6-5
   So r ranges from 0..31, g: 0..63 and b: 0..31
*/
color16_t RGBColor565_565(uint8_t r, uint8_t g, uint8_t b)
{
  return (r << 11 | g << 5 | b);
}

/*
   Returns the color in 16 bit machine readable format when given
   r g and b values in 24 bits. R-G-B 8-8-8
   So r ranges from 0..255, g: 0..255 and b: 0..255
*/
color16_t RGBColor888_565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}