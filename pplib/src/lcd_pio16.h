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

#include "hardware/pio.h"

#if !defined LCD_DOUBLE_PIXEL_NEAREST

	#define lcd_output_wrap_target 0
	#define lcd_output_wrap 3

	#define lcd_output_offset_tx_dat 0u
	#define lcd_output_offset_tx_cmd 2u

	static const uint16_t lcd_output_program_instructions[] = {
				//     .wrap_target
		0x7008, //  0: out    pins, 8         side 0     
		0x1800, //  1: jmp    0               side 1     
		0x7000, //  2: out    pins, 32        side 0     
		0x7878, //  3: out    null, 24        side 1     
				//     .wrap
	};

	static const struct pio_program lcd_output_program = {
		.instructions = lcd_output_program_instructions,
		.length = 4,
		.origin = -1,
	};

	static inline pio_sm_config lcd_output_program_get_default_config(uint offset) {
		pio_sm_config c = pio_get_default_sm_config();
		sm_config_set_wrap(&c, offset + lcd_output_wrap_target, offset + lcd_output_wrap);
		sm_config_set_sideset(&c, 2, true, false);
		return c;
	}

#else

	#define lcd_output_wrap_target 0
	#define lcd_output_wrap 10

	#define lcd_output_offset_tx_dat 0u
	#define lcd_output_offset_tx_cmd 9u

	static const uint16_t lcd_output_program_instructions[] = {
				//     .wrap_target
		0x7028, //  0: out    x, 8            side 0     
		0xa001, //  1: mov    pins, x                    
		0x7848, //  2: out    y, 8            side 1     
		0xb002, //  3: mov    pins, y         side 0     
		0xb842, //  4: nop                    side 1     
		0xb001, //  5: mov    pins, x         side 0     
		0xb842, //  6: nop                    side 1     
		0xb002, //  7: mov    pins, y         side 0     
		0x1800, //  8: jmp    0               side 1     
		0x7000, //  9: out    pins, 32        side 0     
		0x7878, // 10: out    null, 24        side 1     
				//     .wrap
	};

	static const struct pio_program lcd_output_program = {
		.instructions = lcd_output_program_instructions,
		.length = 11,
		.origin = -1,
	};

	static inline pio_sm_config lcd_output_program_get_default_config(uint offset) {
		pio_sm_config c = pio_get_default_sm_config();
		sm_config_set_wrap(&c, offset + lcd_output_wrap_target, offset + lcd_output_wrap);
		sm_config_set_sideset(&c, 2, true, false);
		return c;
	}

#endif
