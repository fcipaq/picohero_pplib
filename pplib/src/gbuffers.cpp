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

#include <Arduino.h>
#include "gbuffers.h"
#include "hardware/interp.h"

uint16_t gbuf_get_width(gbuffer8_t buf) {
  return buf.width;
}

uint16_t gbuf_get_width(gbuffer16_t buf) {
  return buf.width;
}

uint16_t gbuf_get_height(gbuffer8_t buf) {
  return buf.height;
}

uint16_t gbuf_get_height(gbuffer16_t buf) {
  return buf.height;
}

int gbuf_alloc(gbuffer8_t* buf, uint16_t width, uint16_t height, uint8_t colors) {
  buf->data = (uint8_t*)aligned_alloc(4, (width * height) + 4 - (width * height) % 4);  // malloc(width * height);

  if (buf->data == NULL)
    return BUF_ERR_NO_RAM;

  for (uint32_t i = 0; i < width * height; i++)
    buf->data[i] = 0;

  buf->width = width;
  buf->height = height;

  return BUF_SUCCESS;
}

int gbuf_alloc(gbuffer16_t* buf, uint16_t width, uint16_t height, uint8_t colors) {
  buf->data = (uint16_t*)aligned_alloc(4, (width * height * 2) + 4 - (width * height * 2) % 4);  // malloc(width * height * 2);

  if (buf->data == NULL)
    return BUF_ERR_NO_RAM;

  for (uint32_t i = 0; i < width * height; i++)
    buf->data[i] = 0;

  buf->width = width;
  buf->height = height;

  return BUF_SUCCESS;
}

uint8_t* gbuf_get_dat_ptr(gbuffer8_t buf) {
  return buf.data;
}

uint16_t* gbuf_get_dat_ptr(gbuffer16_t buf) {
  return buf.data;
}

void gbuf_free(gbuffer8_t buf) {
  free((void*)buf.data);
}

void gbuf_free(gbuffer16_t buf) {
  free((void*)buf.data);
}
