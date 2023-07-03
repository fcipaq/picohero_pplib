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

#ifndef GBUFFERS_H
#define GBUFFERS_H

/* ========================== includes ========================== */
#include "typedefs.h"

/* ======================== definitions ========================= */
#define BUF_COLORDEPTH_8 8
#define BUF_COLORDEPTH_16 16

// Errors
#define BUF_SUCCESS 0
#define BUF_ERR_NO_RAM -1

/* ==================== function declarations =================== */
uint16_t  gbuf_get_width(gbuffer8_t buf);
uint16_t  gbuf_get_width(gbuffer16_t buf);
uint16_t  gbuf_get_height(gbuffer8_t buf);
uint16_t  gbuf_get_height(gbuffer16_t buf);
int       gbuf_alloc(gbuffer8_t* buf, uint16_t width, uint16_t height, uint8_t colors);
int       gbuf_alloc(gbuffer16_t* buf, uint16_t width, uint16_t height, uint8_t colors);
uint8_t*  gbuf_get_dat_ptr(gbuffer8_t buf);
uint16_t* gbuf_get_dat_ptr(gbuffer16_t buf);
void      gbuf_free(gbuffer8_t buf);
void      gbuf_free(gbuffer16_t buf);

#endif //GBUFFERS_H
