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
 
// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ---------- //
// snd_output //
// ---------- //

#define snd_output_wrap_target 0
#define snd_output_wrap 6

static const uint16_t snd_output_program_instructions[] = {
            //     .wrap_target
    0x80a0, //  0: pull   block                      
    0x7830, //  1: out    x, 16           side 1     
    0xa046, //  2: mov    y, isr                     
    0x00a5, //  3: jmp    x != y, 5                  
    0x1006, //  4: jmp    6               side 0     
    0xa042, //  5: nop                               
    0x0083, //  6: jmp    y--, 3                     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program snd_output_program = {
    .instructions = snd_output_program_instructions,
    .length = 7,
    .origin = -1,
};

static inline pio_sm_config snd_output_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + snd_output_wrap_target, offset + snd_output_wrap);
    sm_config_set_sideset(&c, 2, true, false);
    return c;
}


#endif