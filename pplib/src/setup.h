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
 
#ifndef SETUP_H
#define SETUP_H

/* ------------------------ buttons assignment ------------------------- */
#define BUTTON_PULL_MODE INPUT_PULLDOWN
#define BUTTON_PRESSED HIGH

#define PIN_ANALOG_X  26
#define PIN_ANALOG_Y  27

#define PIN_BUTTON_1  21
#define PIN_BUTTON_2  22
#define PIN_BUTTON_3  20

/* ----------------------- Power management pins ----------------------- */
#define PIN_BAT_ADC   29
//#define BAT_PIN_SRC   24

/* ------------------------- Sound assignment -------------------------- */
#define PIN_SND       0 
//LED_BUILTIN

/* ------------------------ LCD pin assignment ------------------------- */
#define PIN_LCD_BL_PWM 14

#define PIN_LCD_TE    2    // tearing pin
#define PIN_LCD_DC    3    // data/command control pin
#define PIN_LCD_WR    4    // clock
#define PIN_LCD_RST   5    // reset pin

// Note: the data pins are PIO hardware configured and
// driven and need to be in consecutive order
#define PIN_LCD_D0    6     // 1st of the 8 data pins

// SD pins
#define PIN_SD_MISO       16
#define PIN_SD_MOSI       19
#define PIN_SD_SCK        18
#define PIN_SD_CS         17

// LCD_PIO_SPEED defines the PIO speed (in MHz)
// Framerate is calculated as follows: approx. 3.3 fps / MHz
// For a LCD_PIO_SPEED of 66 MHz that means max. 216 fps@66MHz.
// A frequency above 120 MHz is not recommended.
// ILI9341 datasheet states a max. clock frequency of 30 MHz.
#define LCD_PIO_SPEED 30 //120

/* ------------------------- screen orientatin ------------------------- */
// 0 = 0° (portrait), 1 = 90° (landscape), 2 = 180° (portrait), 3 = 270° (landscape)
#define LCD_ROTATION 1

/* ---------------------------- color depth ---------------------------- */
// currently supported are 8 bits and 16 bits of colors
#define LCD_COLORDEPTH 8

/* ------------------ pixel doubling / panel fitting ------------------- */
// please choose 1 or 0 fitters
//#define LCD_DOUBLE_PIXEL_LINEAR  // works with 16 bit mode only
//#define LCD_DOUBLE_PIXEL_NEAREST

/* ---------------------------- TFT driver ----------------------------- */
#include "lcd_drv/ili9341_defines.h"

/* --------------------------- sanity check ---------------------------- */
#if LCD_COLORDEPTH!=8 && LCD_COLORDEPTH!=16
#error Please choose a valid color depth
#endif

#if defined LCD_DOUBLE_PIXEL_LINEAR && defined LCD_DOUBLE_PIXEL_NEAREST
#error Please either choose LCD_DOUBLE_PIXEL_LINEAR or LCD_DOUBLE_PIXEL_NEAREST
#endif

#if defined LCD_DOUBLE_PIXEL_LINEAR && LCD_COLORDEPTH==8
  #error 8 bpp cannot be combined with linear interpolation currently
#endif

#endif //SETUP_H
