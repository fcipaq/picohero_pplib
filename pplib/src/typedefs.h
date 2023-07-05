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
 
#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <Arduino.h>
#include "setup.h"

/* ====================== type definitions ====================== */
/* ------------------------- colors ------------------------- */
typedef uint16_t color_palette_t;

typedef uint16_t color16_t;
typedef uint8_t color8_t;

#if LCD_COLORDEPTH==16
  // 16 bit / 65k colors
  typedef color16_t color_t;
#elif LCD_COLORDEPTH==8
  // 8 bit / 256 colors
  typedef color8_t color_t;
#endif

/* ---------------------- graphics buffers ------------------------- */
typedef struct 
{
    uint8_t bpp;
    uint16_t width;
	uint16_t height;
	color8_t* data;
} gbuffer8_t;

typedef struct
{
    uint8_t bpp;
    uint16_t width;
	uint16_t height;
	color16_t* data;
} gbuffer16_t;

#if LCD_COLORDEPTH==16
  // 16 bit / 65k colors
  typedef gbuffer16_t gbuffer_t;
#elif LCD_COLORDEPTH==8
  // 8 bit / 256 colors
  typedef gbuffer8_t gbuffer_t;
#endif

/* ------------------------- tile maps ------------------------- */
typedef struct 
{
    uint16_t width;
	uint16_t height;
	color8_t* data;
} tile_map_t;

typedef struct 
{
    uint16_t width;
    uint16_t height;
    uint16_t num;	
	gbuffer8_t* image;
} tile_data8_t;

typedef struct 
{
    uint16_t width;
    uint16_t height;
    uint16_t num;	
	gbuffer16_t* image;
} tile_data16_t;

#if LCD_COLORDEPTH==16
  // 16 bit / 65k colors
  typedef tile_data16_t tile_data_t;
#elif LCD_COLORDEPTH==8
  // 8 bit / 256 colors
  typedef tile_data8_t tile_data_t;
#endif

/* ------------------------- coords ------------------------- */
typedef int coord_t;

/* ------------------------- defines ------------------------- */
typedef uint8_t font_t;

#endif // TYPEDEFS_H
