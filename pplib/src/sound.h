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

#ifndef SOUND_H
#define SOUND_H

/* ========================== includes ========================== */
#include "typedefs.h"

/* ========================= definitions ========================= */
// buffer parameters
#define SND_NUM_BUFS 3 
#define SND_BUF_SIZE 1024

// channel definitions
#define SND_NUM_CHAN 4 // CAUTION: SND_NUM_CHAN depending on clock frequency => DO NOT JUST EDIT
#define SND_CHAN_ALL 255
#define SND_CHAN_0 0
#define SND_CHAN_1 1
#define SND_CHAN_2 2
#define SND_CHAN_3 3

// function parameters
#define SND_NONBLOCKING 0
#define SND_BLOCKING 1

// error messages
#define SND_SUCCESS           0
#define SND_CHAN_INVALID     -1
#define SND_NO_RAM           -2
#define SND_NOT_INIT         -3
#define SND_DMA_ERR          -4
#define SND_PIO_ERR          -5
#define SND_NO_FREE_BUF      -6
#define SND_UNDEFINED_ERR  -128

/* ====================== function declarations ====================== */
bool snd_init();

// channel control
int  snd_enque_buf(uint8_t *ext_buf, uint32_t buffersize, uint8_t num_snd_channel, bool blocking);
int  snd_num_bufs_free(uint8_t num_snd_channel);
int  snd_num_bufs_waiting(uint8_t num_snd_channel);
int  snd_cancel_channel(uint8_t num_snd_channel);

// output control
int  snd_set_freq(uint32_t freq);
void snd_set_vol(uint8_t vol);


#endif //SOUND_H
