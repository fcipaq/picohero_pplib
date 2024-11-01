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
#include "../typedefs.h"

/* ==================== function declarations =================== */
/**
 * @brief Returns the color in 16 bit machine readable when given
 *        a color in RGB 8-8-8 format.
 * @note  So r, g and b range from 0..255, g: 0..255 and b: 0..255
 *        The color is returned in 16 bit RGB 5-6-5 format.
 * 
 * @param[in] r red color component
 * @param[in] g green color component
 * @param[in] b blue color component
 *
 * @return  Color in 16 bit format (see note for more details)
 *
 */
color16_t rgb_col_888_565(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Returns the color in 8 bit machine readable when given
 *        a color in RGB 3-3-2 format.
 * @note  So r, g and b range from 0..255, g: 0..255 and b: 0..255
 *        The color is returned in 8 bit RGB 3-3-2 format.
 *        This only works if no custom color palette is being used.
 * 
 * @param[in] r red color component
 * @param[in] g green color component
 * @param[in] b blue color component
 *
 * @return  Color in 8 bit format (see note for more details)
 *
 */
 color8_t  rgb_col_888_332(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Returns the color in 16 bit machine readable when given
 *        a color in RGB 5-6-5 format.
 * @note  So r, g and b range from 0..31, g: 0..63 and b: 0..31
 *        The color is returned in 16 bit RGB 5-6-5 format.
 * 
 * @param[in] r red color component
 * @param[in] g green color component
 * @param[in] b blue color component
 *
 * @return  Color in 16 bit format (see note for more details)
 *
 */
color16_t rgb_col_565_565(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Returns the color in 8 bit machine readable when given
 *        a color in RGB 3-3-2 format.
 * @note  So r, g and b range from 0..7, g: 0..7 and b: 0..3
 *        The color is returned in 8 bit RGB 3-3-2 format.
 *        This only works if no custom color palette is being used.
 * 
 * @param[in] r red color component
 * @param[in] g green color component
 * @param[in] b blue color component
 *
 * @return  Color in 8 bit format (see note for more details)
 *
 */
color8_t rgb_col_332_332(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Returns the red component of a RGB565 color.
 * @note  Return value is ranges from 0..31
 * 
 * @param[in] col color in RGB 5-6-5 format
 *
 * @return Red component of a RGB565 color.
 *
 */
uint8_t rgb_col_565_red(color16_t col);

/**
 * @brief Returns the green component of a RGB565 color.
 * @note  Return value is ranges from 0..31
 * 
 * @param[in] col color in RGB 5-6-5 format
 *
 * @return Green component of a RGB565 color.
 *
 */
uint8_t   rgb_col_565_green(color16_t col);
 
/**
 * @brief Returns the blue component of a RGB565 color.
 * @note  Return value is ranges from 0..31
 * 
 * @param[in] col color in RGB 5-6-5 format
 *
 * @return Blue component of a RGB565 color.
 *
 */
uint8_t   rgb_col_565_blue(color16_t col);

#endif // COLORS_H
