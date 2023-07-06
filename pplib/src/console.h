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

#pragma once

/* ========================== includes ========================== */
#include <Arduino.h>
#include "typedefs.h"

#define MAX_LINES 30
#define MAX_COLUMNS 50

class console {
  private:
    char* _textbuffer;

    font_t* _font = NULL;
    int _font_height = 0;

    gbuffer_t _buf;
    int _pos_x = 0;
    int _pos_y = 0;
    int _width = 0;
    int _height = 0;

    color_t _col = 0;
    color_t _bkg_col = 0;

    int _num_lines = 0;
    int _max_lines = MAX_LINES;
    int _max_columns = MAX_COLUMNS;

  public:
    console(coord_t x, coord_t y, uint16_t width, uint16_t height, color_t text_col, color_t bkg_col, font_t* font, gbuffer_t buf);
    ~console(void);

	void set_text_col(color_t col);
    void set_bkg_col(color_t col);
	void set_buffer(gbuffer_t buf);
    void set_font(font_t* font);
    void set_coords(coord_t x, coord_t y, uint16_t width, uint16_t height);
    void print(char* str);
    void update();
};