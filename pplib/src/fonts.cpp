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

#include "fonts.h"
#include "gbuffers.h"

#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>

/* ======================== definitions ========================= */
#define FONT_HEADER_FNT_WIDTH  2
#define FONT_HEADER_FNT_HEIGHT 3
#define FONT_HEADER_FRST_CHAR  5
#define FONT_HEADER_LST_CHAR   6
#define FONT_HEADER_CHAR_WIDTH 7

/* ========================= implementation ========================== */

int font_get_width(font_t* font) {
  return font[FONT_HEADER_FNT_WIDTH];
}

int font_get_height(font_t* font) {
  return font[FONT_HEADER_FNT_HEIGHT];
}

int font_get_char_width(char c, font_t* font) {
  if (c < font[FONT_HEADER_FRST_CHAR] || c > font[FONT_HEADER_LST_CHAR])
    return 0;

  c -= font[FONT_HEADER_FRST_CHAR];
  return font[FONT_HEADER_CHAR_WIDTH + c];
}

uint16_t font_get_data_ofs(font_t* font) {
  return FONT_HEADER_CHAR_WIDTH + font[FONT_HEADER_LST_CHAR] - font[FONT_HEADER_FRST_CHAR] + 1;
}

void font_put_char(coord_t pos_x, coord_t pos_y, color_t col, char c, font_t* font, gbuffer_t dst) {
  uint8_t charWidth = font_get_char_width(c, font);

  // character not defined
  if (charWidth == 0)
    return;

  if (c < font[FONT_HEADER_FRST_CHAR] || c > font[FONT_HEADER_LST_CHAR])
    return;

  uint8_t charHeight = font[FONT_HEADER_FNT_HEIGHT];
  uint16_t bufWidth = gbuf_get_width(dst);
  uint16_t bufHeight = gbuf_get_height(dst);
    
  uint8_t charnum = c - font[FONT_HEADER_FRST_CHAR];

  uint32_t total_width = 0;
  
  for (char i = font[FONT_HEADER_FRST_CHAR]; i < c; i++)
      total_width += font_get_char_width(i, font);

  uint32_t total_bits = (total_width * charHeight);

  uint16_t data_ofs = font_get_data_ofs(font) + total_bits / 8;

  uint8_t bitcnt = (uint32_t) total_bits % 8;
  uint8_t datcnt = 0;

  char fnt_dat = font[data_ofs];

   for (uint16_t x = 0; x < charWidth; x++)
    for (uint16_t y = 0; y < charHeight; y++)
    {
      if (bitcnt == 8) {
        bitcnt = 0;
        datcnt++;
        fnt_dat = font[data_ofs + datcnt];
      }
            
      if ((pos_y + y) >= 0 && (pos_y + y) < bufHeight &&
          (pos_x + x) >= 0 && (pos_x + x) < bufWidth) {
        if (fnt_dat & (1 << bitcnt))
          dst.data[(pos_y + y) * bufWidth + (pos_x + x)] = col;
      }
      
      bitcnt++;
    }
	
}

void font_write_string(coord_t pos_x, coord_t pos_y, color_t col, char* str, font_t* font, gbuffer_t dst) {
  int h = 0;
  int ofs_x = 0;
  
  while (str[h] != 0) {
    font_put_char(pos_x + ofs_x, pos_y, col, str[h], font, dst);
    ofs_x += font_get_char_width(str[h], font) + 1;
    h++;
  }
}

int font_get_string_width(char* str, font_t* font) {
  int h = 0;
  uint16_t len = 0;
  
  while (str[h] != 0) {
    len += font_get_char_width(str[h], font) + 1;
    h++;
  } 

  return len;
}

void font_write_string_centered(coord_t pos_x, coord_t y, color_t col, char* str, font_t* font, gbuffer_t dst) {
  font_write_string(pos_x - font_get_string_width(str, font) / 2, y, col, str, font, dst);
}