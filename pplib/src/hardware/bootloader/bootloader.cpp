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

#include "../controls/controls.h"
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
