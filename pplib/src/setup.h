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
 
#ifndef SETUP_H
#define SETUP_H

// LCD_PIO_SPEED defines the PIO speed (in MHz)
// Framerate is calculated as follows: approx. 3.3 fps / MHz
// For a LCD_PIO_SPEED of 66 MHz that means max. 216 fps@66MHz.
// A frequency above 120 MHz is not recommended.
// ILI9341 datasheet states a max. clock frequency of 30 MHz.
#define LCD_PIO_SPEED 120

/* ------------------------ screen orientatin ------------------------*/
// 0 = 0° (portrait), 1 = 90° (landscape), 2 = 180° (portrait), 3 = 270° (landscape)
#define LCD_ROTATION 1

/* ---------------------- color depth ----------------------*/
// currently supported are 8 bits and 16 bits of colors
#define LCD_COLORDEPTH 8

/* ----------------- pixel doubling / panel fitting -----------------*/
// please choose 1 or 0 fitters
//#define LCD_DOUBLE_PIXEL_LINEAR  // works with 16 bit mode only
#define LCD_DOUBLE_PIXEL_NEAREST

/* ========================= sanity check ========================= */
#if LCD_COLORDEPTH!=8 && LCD_COLORDEPTH!=16
#error Please choose a valid color depth
#endif

#if defined LCD_DOUBLE_PIXEL_LINEAR && defined LCD_DOUBLE_PIXEL_NEAREST
#error Please either choose LCD_DOUBLE_PIXEL_LINEAR or LCD_DOUBLE_PIXEL_NEAREST or no panel fitter at all
#endif

#if defined LCD_DOUBLE_PIXEL_LINEAR && LCD_COLORDEPTH==8
  #error 8 bpp cannot be combined with linear interpolation panel fitter currently
#endif

#endif //SETUP_H
