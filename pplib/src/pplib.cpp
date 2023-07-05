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

#include "pplib.h"

int pplInit() {
  ctrl_init();
  
  bool blreq = checkBootsel();

  if ((lcd_init() != LCD_SUCCESS) && (!blreq))
    return -1;

  if (blreq)
    blMenu();

  if (snd_init() != SND_SUCCESS)
    return -1;
	
  if (pwr_init() != 0)
    return -1;
  
  return 0;
}

