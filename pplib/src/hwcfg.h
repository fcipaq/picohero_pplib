/*
 * pplib - a library for the PicoPal handheld
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
 
#ifndef HWCFG_H
#define HWCFG_H

/*----------------- buttons assignment --------------- */
#define BUTTON_PULL_MODE INPUT_PULLDOWN
#define BUTTON_PRESSED HIGH

#define PIN_ANALOG_X  26
#define PIN_ANALOG_Y  27

#define PIN_BUTTON_1  21
#define PIN_BUTTON_2  22
#define PIN_BUTTON_3  20

/* ------------------------ Power management pins ------------------------*/
#define PIN_BAT_ADC   29
//#define BAT_PIN_SRC   24

/* ------------------------ Sound assignment ------------------------*/
#define PIN_SND       0 

/* ------------------------ LCD pin assignment ------------------------*/
#define PIN_LCD_BL_PWM 14

#define PIN_LCD_TE    2    // tearing pin
#define PIN_LCD_DC    3    // data/command control pin
#define PIN_LCD_WR    4    // clock
#define PIN_LCD_RST   5    // reset pin

// Note: the data pins are PIO hardware configured and driven
// and need to be in consecutive order
#define PIN_LCD_D0    6     // 1st of the 8 data pins

// SD pins
#define PIN_SD_SCK        18
#define PIN_SD_CS         17
#define PIN_SD_MOSI       19
#define PIN_SD_MISO       16

/* ---------------------- TFT driver ----------------------*/
#define LCD_DRIVER_ILI9341
#include "lcd_drv/ili9341_drv.h"

#endif //HWCFG_H
