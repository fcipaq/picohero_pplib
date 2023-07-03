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

#ifndef BUTTONS_H
#define BUTTONS_H

/* ========================== includes ========================== */
#include <Arduino.h>

/* ========================= definitions ========================= */
/* --------------------- software assignment -------------------- */
#define DPAD_NONE     0
#define DPAD_UP       1
#define DPAD_LEFT     2
#define DPAD_RIGHT    4
#define DPAD_DOWN     8

#define BUTTON_NONE   0
#define BUTTON_1      1
#define BUTTON_2      2
#define BUTTON_3      4
#define BUTTON_4      8

/* ====================== function declarations ====================== */
int ctrl_init();

void recalibAnalog();

uint16_t getDPad();
uint16_t getButtons();
int8_t getAnalogX();
int8_t getAnalogY();

#endif // BUTTONS_H