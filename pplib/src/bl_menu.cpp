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

#include "bl_menu.h"
#include "setup.h"
#include "gbuffers.h"
#include "lcdcom.h"
#include "controls.h"
#include "colors.h"

#include "bl_image.h"

#include "pico/bootrom.h"

#include <Arduino.h>

// Enters bootloader in case all buttons are pressed during system reset
bool bl_check_selection() {
  uint16_t buttons = ctrl_button_state();

  if ( (buttons & BUTTON_1) && 
       (buttons & BUTTON_2) && 
	   (buttons & BUTTON_3) )
    return true;
  else
	return false;
}

// Launches the bootloader
void bl_launch_bl() {
	reset_usb_boot(0, 0);
}

void bl_reboot_machine() {
  // let's just hope noone messed with the WD settings...
  watchdog_reboot(0, 0, 1);
  watchdog_enable(1, false);
}

void bl_menu() {
  // Code is quite hacky to support both 8 and 16 bit mode,
  // it bypassed most of the higher level functions of the lcdcom lib
  //#define RGB888_565(r, g, b) ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

// DEBUG
//bl_launch_bl();

  uint32_t timer_te = millis();
  
  while (!lcd_get_vblank()) {
    // No TE signal detected after 10ms
    if (millis() - timer_te > 10)
      break;
  } 

  // If no TE signal can be detected, assume the whole LCD is not connected
  if (millis() - timer_te > 10)
    bl_launch_bl();

  // allocate line buffer
  color_t* lb1 = (color_t*) malloc(lcd_get_screen_width() * lcd_get_screen_bpp() / 2);
  color_t* lb2 = (color_t*) malloc(lcd_get_screen_width() * lcd_get_screen_bpp() / 2);
  // If no linebuffer can be allocated or no LCD connection can be confirmed,
  // skip the menu and launch bootloader right away
  if ((lb1 == NULL) || (lb2 == NULL))
	bl_launch_bl();

  color_t* lb_back = lb1;

  int xstar[BL_NUMSTAR];
  int ystar[BL_NUMSTAR];
  int sstar[BL_NUMSTAR];

  for (int h = 0; h < BL_NUMSTAR; h++) {
    xstar[h] = random(lcd_get_screen_width());
    ystar[h] = random(lcd_get_screen_height());
    sstar[h] = random(3) + 1;
  }

  // initialize selection menu
  int sel = 1;
  int max_sel = 2;

  // debounce section
  uint32_t timer_dpad = millis();
  uint32_t timer_deb = millis(); 
  bool button_debounce = false;

  // fade in animation
  uint8_t fadein_bl_level = 0;
  uint32_t logo_timer = millis();
  bool fadein = true;

  // color of menu items
  color_t text1, text2, text3;
  
  #if lcd_get_screen_bpp()==16
  color_t backgnd = rgb_col_888_565(0x00, 0x00, 0x00);
  color_t textbkg = rgb_col_888_565(200, 60, 255);
  color_t textcol = rgb_col_888_565(0xff, 0xff, 0xff);
  color_t texthi = rgb_col_888_565(0xff, 0x00, 0x00);
  color_t starcol1 = rgb_col_888_565(0xff, 0xff, 0xff);
  color_t starcol2 = rgb_col_888_565(150, 150, 150);
  color_t starcol3 = rgb_col_888_565(80, 80, 80);
  #elif lcd_get_screen_bpp()==8
  color_t backgnd = rgb_col_888_332(0x00, 0x00, 0x00);
  color_t textbkg = rgb_col_888_332(200, 60, 255);
  color_t textcol = rgb_col_888_332(0xff, 0xff, 0xff);
  color_t texthi = rgb_col_888_332(0xff, 0x00, 0x00);
  color_t starcol1 = rgb_col_888_332(0xff, 0xff, 0xff);
  color_t starcol2 = rgb_col_888_332(150, 150, 150);
  color_t starcol3 = rgb_col_888_332(80, 80, 80);
  #endif
  
  // menu position
  #if defined LCD_DOUBLE_PIXEL_NEAREST || defined LCD_DOUBLE_PIXEL_LINEAR
    uint8_t color_corr = 2;
  #else
    uint8_t color_corr = 1;
  #endif

  // bl_menu_image[0] is the width
  // bl_menu_image[1] is the height
  int ofs_x = lcd_get_screen_width() / 2 - bl_menu_image[0] / 2 / color_corr;
  int ofs_y = 30 / color_corr;//lcd_get_screen_height() / 2 - gbuf_get_width((gbuffer_t*) bl_menu_image) / 2;

  while (!ctrl_button_state() || !button_debounce) {
    if (!button_debounce) {
      if (ctrl_button_state()) {
        timer_deb = millis();
      } else {
        if (millis() - timer_deb > 100)
          button_debounce = true;
      }
    }
    
    switch (sel) {
      case 1:
        text1 = texthi;
        text2 = textcol;
        text3 = textcol;
        break;
      case 2:
        text1 = textcol;
        text2 = texthi;
        text3 = textcol;
        break;
      case 3:
        text1 = textcol;
        text2 = textcol;
        text3 = texthi;
        break;
      default:
        break;
    }
           
    for (int h = 0; h < BL_NUMSTAR; h++) {
      xstar[h] -= sstar[h];
      if (xstar[h] < 0)
        xstar[h] = lcd_get_screen_width();
    }

    for (int y = 0; y < lcd_get_screen_height(); y++) {
      // clear line buffer
      for (int h = 0; h < lcd_get_screen_width(); h++)
        lb_back[h] = 0x00;

      // draw stars  
      for (int h = 0; h < BL_NUMSTAR; h++)
        if (ystar[h] == y) {
          if (sstar[h] == 1)
            lb_back[xstar[h]] = starcol3;
          else if (sstar[h] == 2)
            lb_back[xstar[h]] = starcol2;
          else if (sstar[h] == 3)
            lb_back[xstar[h]] = starcol1;
        }

      // draw menu
      for (int x = 0; x < lcd_get_screen_width(); x++) {
        if ( (x >= ofs_x) &&
             (x < ofs_x + bl_menu_image[0] / color_corr) &&
             (y >= ofs_y ) &&
             (y < ofs_y + bl_menu_image[1] / color_corr ) ) {
          uint8_t c = bl_menu_image[ (y - ofs_y) * bl_menu_image[0] * color_corr + (x - ofs_x) * color_corr];

          switch(c) {
            case 0:
              //lb[x] = backgnd;
              break;
            case 1:
              lb_back[x] = textbkg;
              break;
            case 2:
              lb_back[x] = text2;
              break;
            case 3:
              lb_back[x] = textbkg;
              break;
            case 4:
              lb_back[x] = text1;
              break;
            case 5:
              lb_back[x] = textbkg;
              break;
            case 6:
              lb_back[x] = textcol;
              break;
            default:
              break;
          }
        }
      }
	  
      #if defined LCD_DOUBLE_PIXEL_NEAREST || defined LCD_DOUBLE_PIXEL_LINEAR
	    // double scanout each line
        for (int i = 0; i < 2; i++)
          lcd_send_framebuffer(lb_back, lcd_get_screen_width());
	  #else
        lcd_send_framebuffer(lb_back, lcd_get_screen_width());
	  #endif
  
      if (lb_back == lb1)
        lb_back = lb2;
      else
        lb_back = lb1;
    }

    uint16_t dpad = ctrl_dpad_state();

    if (dpad) {
      if (millis() - timer_dpad > 350) {
        timer_dpad = millis();
      } else {
        dpad = 0;
      }
    }
      
    if (dpad & DPAD_UP)
      sel--;
    if (dpad & DPAD_DOWN)
      sel++;
    
    if (sel < 1)
      sel = max_sel;
    if (sel > max_sel)
      sel = 1;

    // fade in animation
    if (fadein && (millis() - logo_timer > BL_ANI_SPEED)) {
      logo_timer = millis();
      fadein_bl_level++;
      lcd_set_backlight(fadein_bl_level);
      if (fadein_bl_level >= BL_BRIGHTNESS)
        fadein = false;
    }

  }

  // crashing here - SDK bug?
  //gbuf_free(lb1);
  //gbuf_free(lb2);

  // fade out
  for (int h = 0; h < BL_BRIGHTNESS; h+=2) {
    lcd_set_backlight(BL_BRIGHTNESS - h);
    delay(BL_ANI_SPEED / 4);
  }

  switch (sel) {
    case 1:
	 bl_launch_bl();
     break;
    case 2:
     bl_reboot_machine();
     break;
    default:
    break;
  }

}

