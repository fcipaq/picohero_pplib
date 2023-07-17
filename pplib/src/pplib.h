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
#include <lcdcom.h>
#include <sound.h>
#include <controls.h>
#include <power.h>
#include <setup.h>

// Bootloader
#include <bl_menu.h>

// Graphics
#include <gbuffers.h>
#include <colors.h>
#include <primitives.h>
#include <fonts.h>
#include <blitter.h>
#include <tilemap.h>

/* ======================== definitions ========================= */

// error messages
#define PPL_SUCCESS           0
#define PPL_UNKNOWN_ERROR    -1

/* ====================== function declarations ====================== */
int ppl_init();

#endif // BLITTER
