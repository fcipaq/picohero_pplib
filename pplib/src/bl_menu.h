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

#ifndef BL_MENU_H
#define BL_MENU_H

/* ========================= definitions ========================= */
#define BL_NUMSTAR 100
#define BL_BRIGHTNESS 20
#define BL_ANI_SPEED 50

/* ====================== function declarations ====================== */
void bl_menu();
void bl_launch_bl();
bool bl_check_selection();

#endif // BL_MENU_H