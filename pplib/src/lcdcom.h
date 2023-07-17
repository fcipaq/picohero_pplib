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
 
#ifndef LCDCOM_H
#define LCDCOM_H

/* ========================== includes ========================== */
#include "typedefs.h"
#include "hwcfg.h"

/* ======================== definitions ========================= */

// error messages
#define LCD_SUCCESS           0
#define LCD_UNKNOWN_ERROR    -1
#define LCD_NOT_INIT         -3
#define LCD_DMA_ERR          -4
#define LCD_PIO_ERR          -5

/* --------------------- screen mode handling --------------------*/
/*
#if defined LCD_DOUBLE_PIXEL_LINEAR || defined LCD_DOUBLE_PIXEL_NEAREST
  #if LCD_ROTATION==0 || LCD_ROTATION==2
    #define SCREEN_WIDTH   (PHYS_SCREEN_WIDTH / 2)
    #define SCREEN_HEIGHT  (PHYS_SCREEN_HEIGHT / 2)
  #elif LCD_ROTATION==1 || LCD_ROTATION==3
    #define SCREEN_WIDTH   (PHYS_SCREEN_HEIGHT / 2)
    #define SCREEN_HEIGHT  (PHYS_SCREEN_WIDTH / 2)
  #endif
#else
  #if LCD_ROTATION==0 || LCD_ROTATION==2
    #define SCREEN_WIDTH   PHYS_SCREEN_WIDTH
    #define SCREEN_HEIGHT  PHYS_SCREEN_HEIGHT
  #elif LCD_ROTATION==1 || LCD_ROTATION==3
    #define SCREEN_WIDTH   PHYS_SCREEN_HEIGHT
    #define SCREEN_HEIGHT  PHYS_SCREEN_WIDTH
  #endif
#endif
*/

#define lcd_get_screen_width() (SCREEN_WIDTH)
#define lcd_get_screen_height() (SCREEN_HEIGHT)
#define lcd_get_screen_bpp() (LCD_COLORDEPTH)

/* ====================== function declarations ====================== */
int  lcd_init();
void lcd_set_speed(uint32_t freq);

/* ---------------------- LCD data transmission ---------------------- */
int  lcd_send_framebuffer(void* buf, uint32_t buffersize);
int  lcd_show_framebuffer(gbuffer_t buf);
void lcd_wait_ready();
int  lcd_check_ready();

/* ------------------------ LCD hardware ctrl -------------------------*/
void lcd_set_backlight(byte level);

void lcd_set_addr(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void lcd_enable_te();
void lcd_disable_te();
bool lcd_get_vblank();

#if LCD_COLORDEPTH == 8
color_palette_t* lcd_get_palette_ptr();
void lcd_std_palette();
#endif

#endif //LCD_COM_H
