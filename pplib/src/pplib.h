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
 
#ifndef PPLIB_H
#define PPLIB_H

/* ========================== includes ========================== */
// HAL
#include "hardware/lcd_if/lcdcom.h"
#include "hardware/sound/sound.h"
#include "hardware/controls/controls.h"
#include "hardware/power/power.h"
#include "hardware/bootloader/bootloader.h"
#include "setup.h"

// Graphics
#include "graphics/gbuffers.h"
#include "graphics/colors.h"
#include "graphics/primitives.h"
#include "graphics/blitter.h"
#include "graphics/tilemap.h"
#include "fonts/fonts.h"

/* ======================== definitions ========================= */

#define PPL_VERSION 241101

// error messages
#define PPL_SUCCESS           0
#define PPL_UNKNOWN_ERROR    -1

/* ====================== init ====================== */
int ppl_init() {
  ctrl_init();
  
  bool bl_req = bl_check_selection();

  if (bl_req)
    bl_launch_bl();

  if (lcd_init() != LCD_SUCCESS)
    return PPL_UNKNOWN_ERROR;

/*
  if ((lcd_init() != LCD_SUCCESS) && (!bl_req))
    return PPL_UNKNOWN_ERROR;

  if (bl_req)
    bl_menu();
*/

#ifndef PPL_SND_INIT_MANUAL
  if (snd_init() != SND_SUCCESS)
    return PPL_UNKNOWN_ERROR;
#endif

  if (pwr_init() != 0)
    return PPL_UNKNOWN_ERROR;
  
  return PPL_SUCCESS;
}

#endif // BLITTER
