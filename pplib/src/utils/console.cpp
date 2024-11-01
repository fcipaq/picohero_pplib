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

#include "console.h"
#include "../graphics/gbuffers.h"
#include "../graphics/primitives.h"
#include "../fonts/fonts.h"

  console::console(coord_t x, coord_t y, uint16_t width, uint16_t height, color_t text_col, color_t bkg_col, font_t* font, gbuffer_t buf) {
    set_text_col(text_col);
    set_bkg_col(bkg_col);
    set_coords(x, y, width, height);
    set_font(font);
	_buf = buf;
	_textbuffer = (char*) malloc(_max_lines * _max_columns);
    memset(_textbuffer, 0, _max_lines * _max_columns);
  }

  console::~console(void) {
    free((void*) _textbuffer);
  }

  void console::set_text_col(color_t col) {
    _col = col;
  }

  void console::set_bkg_col(color_t col) {
    _bkg_col = col;
  }

  void console::set_buffer(gbuffer_t buf) {
    _buf = buf;
  }

  void console::set_font(font_t* font) {
    _font = font;
    _font_height = font_get_height(font);
    _num_lines = _height / (_font_height + 4);

    if (_num_lines > _max_lines)
      _num_lines = _max_lines;
  }

  void console::set_coords(coord_t x, coord_t y, uint16_t width, uint16_t height) {
    _pos_x = x;
    _pos_y = y;
    _width = width;
    _height = height;
  }

  void console::print(float f) {
    char str[11];
    itoa(f, str, 10);

	for (int h = _num_lines - 1; h > 0; h--)
      for (int i = 0; i < _max_columns; i++)
        _textbuffer[h * _max_columns + i] = _textbuffer[(h - 1) * _max_columns + i];

    for (int i = 0; i < _max_columns - 1; i++) {
      _textbuffer[i] = str[i];
      if (str[i] == 0)
        break;
    }

    _textbuffer[_max_columns - 1] = 0;
  }

  void console::print(char* str) {
	for (int h = _num_lines - 1; h > 0; h--)
      for (int i = 0; i < _max_columns; i++)
        _textbuffer[h * _max_columns + i] = _textbuffer[(h - 1) * _max_columns + i];

    for (int i = 0; i < _max_columns - 1; i++) {
      _textbuffer[i] = str[i];
      if (str[i] == 0)
        break;
    }

    _textbuffer[_max_columns - 1] = 0;
  }

  void console::update() {
    draw_rect_fill(_pos_x, _pos_y, _pos_x + _width, _pos_y + _height, _bkg_col, _buf);
    for (int h = 0; h < _num_lines; h++)
      font_write_string(_pos_x, _pos_y + _height - (h + 1) * (_font_height + 4) - 2, _col, (char*) &_textbuffer[h * _max_columns], _font, _buf);
  }

