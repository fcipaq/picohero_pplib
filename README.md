### pplib - A library for the ~~Pico Pad~~ *Pico Hero* (an open source handheld based on the Raspberry Pi Pico) for Arduino

This is the result of a private project to build a handheld gaming device based on the
*Raspberry Pi Pico*. I know there are a bunch of those out there but I wanted to design my
own for some reasons:

 - I like DIY stuff rather than just buying something off the shelf
 - a bigger screen (3.2 inches - a lot of the devices available are only 2 inches)
 - a parallel connection to the screen (for fast refresh rate)
 - a decent speaker
 - an (I hope not only for me) appealing design

And this is what it looks like:

![Pico Hero picture 1](https://raw.githubusercontent.com/fcipaq/picohero_pplib/main/images/front1.jpg)
![Pico Hero picture 1](https://raw.githubusercontent.com/fcipaq/picohero_pplib/main/images/front3.jpg)

Front view

![Pico Hero picture 1](https://raw.githubusercontent.com/fcipaq/picohero_pplib/main/images/back.jpg)

Back view

![Pico Hero picture 1](https://raw.githubusercontent.com/fcipaq/picohero_pplib/main/images/top.jpg)


Top view

![Pico Hero picture 1](https://raw.githubusercontent.com/fcipaq/picohero_pplib/main/images/parts.jpg)

Parts (older version without SD card)

![Pico Hero picture 1](https://raw.githubusercontent.com/fcipaq/picohero_pplib/main/images/emulator.jpg)

*Pico Hero* running modified Sega Mega Drive emulator by bzhxx (https://github.com/bzhxx/gwenesis)

I will add more repositories, tools and details in time. Also, I am going to add Gerber files and schematics as well as STL files for printing/building the case. Consider this as preview. Please be patient, I am going at full speed.

# Specs:

- 3.2 inch LCD screen with 320x240 resolution with parallel interface to the RP2040
- analog control stick, 3 buttons and on/off switch
- 3W speaker
- micro SD card slot (for space constraints it’s push/pull so a it’s a little fiddly to remove the SD card) 
- size: 136mm x 60mm x 12mm; weight: 100g including 600 mAh battery
- optional charging circuitry when using a RPi Pico module without charger logic.

I originally intended to call the handheld *Pico Pad* – however a Czech company was a couple days faster.

Now, the device features a simple library which does the hardware handling. The library is for *Arduino* so you just need to put it in your libraries folder and can start developing right away.

The whole library is built around Earle Philhower's Raspberry Pi Pico Arduino core (no further dependencies are involve). 

# Core features:

- screen: 16 bit and 8 bit (must choose at compile time) buffers, full and half resolution panel fitter (nearest neighbor and experimental linear filter)
- fast blitter (using the RP2040 interpolator) with zooming/rotation
- fast tilemap routines to build platformer, racing games, RPGs etc. (also using the RP2040 interpolator)
- sound library for sound output (8 bit PCM up to 22000 Hz) with four channels for mixing
- font library with the ability to import true type fonts (FontEditor written by H. Reddmann)

# Requirements

RAM

- Display:
  - at least 19 KB RAM (8 bit, 160x120, single FB) up to 154 KB (16 bit, 320x240, single FB)   
  - 512 bytes of scratch-x memory when using 8 bit mode (for the palette LUT)

- Sound:
  - typically 3 KB sound buffer (depends on config)

- PIO
  - 2 state machines from PIO0 in 16 bit mode and one additional state machine when using 8 bit mode

- DMA
  - 2 DMA channels (2 additional DMA channels when using 8 bit and custom color palette)

### Installation

Place the *pplib* directory into your Arduino libraries folder. Next, install Earle Philhower's Raspberry Pi Pico Arduino core. The library is meant to be used with Arduino. It is written in non-object-oriented C++. At first you need to setup the hardware (16 bit or 8 bit color depth, resolution, panel fitter). See setup.h for details. Your sketch has then to call pplInit() right at the beginning. 
That’s it. You may now build the examples.

### Library description:

# Types

// TODO


## Functions

# pplib

`int pplInit()`

This function is called to initialize all the hardware on the *Pico Hero*. It’s supposed to be called right at the beginning of `setup()`. This also does the bootloader handling (press all three buttons when powering up the *Pico Hero* to enter bootloader mode) – so it’s very important. 


# gbuffers

All graphics are rendered into objects called graphics buffers (or gbuffers). Those contain the information about the image size and color depth as well as a pointer to the actual image data.
The functions are overloaded to work with 8 bit as well as 16 bit color depths.
The library does not support rendering graphics (e.g. a line or a circle) directly to the screen.

`uint16_t  gbuf_get_width(gbuffer8_t buf)`

Returns the width of a buffer

`uint16_t  gbuf_get_height(gbuffer8_t buf)`

Returns the height of a buffer

`int gbuf_alloc(gbuffer8_t* buf, uint16_t width, uint16_t height, uint8_t colors) `

Allocates memory to an already existing graphics buffer object. Returns *BUF_ERR_NO_RAM* on failure otherwise * BUF_SUCCESS*

`uint8_t*  gbuf_get_dat_ptr(gbuffer8_t buf)`

Returns the data pointer of a buffer object.

`void gbuf_free(gbuffer8_t buf) `

Frees the data memory of a buffer object.



# lcdcom

This is the hardware layer to interface with the LCD

`void lcd_set_speed(uint32_t freq)`
This sets the LCD interface speed to the ILI9341. Specs state a max frequency of approx. 30 MHz however 100 MHz should work fine. You should not call this in the middle of program execution (it’s called from *pplInit*). When the CPU is being overclocked, the function is aware of that and the PIO speed adjusted accordingly.

`int  lcd_show_framebuffer(gbuffer_t buf)`

Sends a graphics buffer to the LCD.

`void lcd_wait_ready()`

Waits until a pending buffer has been sent to the LCD.

`int  lcd_check_ready()`

Returns *true* if the transmission subsystem is idle (i.e. is ready to send another buffer).

`void lcd_set_backlight(byte level)`

Sets the backlight level (from 0% to 100%)

`void lcd_enable_te()`

Enables the tearing signal.

`void lcd_disable_te()`

Disables the tearing signal.

`bool lcd_get_vblank()`

Returns the current state of the tearing signal. 


# sound

`int  snd_enque_buf(uint8_t *ext_buf, uint32_t buffersize, uint8_t num_snd_channel, bool blocking) `

Enques a buffer to be played. *ext_buf* is a pointer to an unsigned 8 bit array of a PCM buffer. You may state one of four channels (SND_CHAN_0 to SND_CHAN_3). If _blocking_ is set to SND_BLOCKING then the funciont will only return once the buffer is enqueued (if there is no free buffer, the function waits). If set to SND_NONBLOCKING, the function will immediately return even if the buffer was unable to be enqueued. If the buffer was successfully enqueued *SND_SUCCESS* is returned.
The buffer will be played with support of DMA and interrupts. So this is mostly fire-and-forget. Once the buffer is enqueued, it will be played without any further action needed.

`int  snd_num_bufs_free(uint8_t num_snd_channel)`

Returns the number of free buffers.

`int  snd_num_bufs_waiting(uint8_t num_snd_channel) `

Returns the number of buffers waiting (0 means idle)

`int  snd_cancel_channel(uint8_t num_snd_channel)`

Cancels playback on given channel (*SND_CHAN_ALL* cancels all playback)

`int  snd_set_freq(uint32_t freq)`

Sets the playback frequency.

`void snd_set_vol(uint8_t vol)`

Sets the volume level (from 0 to 5)


# blitter

The blitter blits a source buffer to a destination buffer. The buffers may be different in size but must be of the same color depth. There is only one (overloaded) function.

```
void blit_buf(coord_t kx,    // coordinates of upper left corner
			  coord_t ky,    
			  gbuffer_t src, // source buffer
			  gbuffer_t dst, // destination buffer
			  int32_t alpha)
```

Blits a buffer to another buffer at the position *kx*, *ky*. Alpha states the transparent color (BLIT_NO_ALPHA for no transparency)


```
void blit_buf(coord_t kx,		// kx: x-coord where to blit the of CENTER of the image
			  coord_t ky,		// ky: y-coord where to blit the of CENTER of the image
			  float zoom,			// zoom: zoom factor (same in both directions) 
		      float rot,             // rot: rotation of the image (in rad)
			  int32_t alpha,         // alpha: color which is NOT being drawn (BLIT_NO_ALPHA for no transparency)
			  gbuffer_t src,   // sprite: pointer to source buffer
			  gbuffer_t dst)     // fb: pointer to destination buffer
```

Blits a buffer to another buffer at the position *kx*, *ky*, zooms it at the factor of *zoom* and rotates is at the angle of *rot*. Alpha states the transparent color (BLIT_NO_ALPHA for no transparency)



```
void blit_buf(coord_t kx,		// kx: x-coord where to blit the of CENTER of the image
			  coord_t ky,		// ky: y-coord where to blit the of CENTER of the image
			  float zoom_x,			// zoom_x: zoom factor in x direction
			  float zoom_y,			// zoom_y: zoom factor in y direction
			  uint8_t flip_x,		// flip_x: whether to flip horizontally (1 means flip, 0 means
			  uint8_t flip_y,		// flip_y: whether to flip vertically
			  int32_t alpha,         // alpha: color which is NOT being drawn (BLIT_NO_ALPHA for no transparency)
			  gbuffer_t src,   // sprite: pointer to source buffer
			  gbuffer_t dst)      // fb: pointer to destination buffer
```

Blits a buffer to another buffer at the position *kx*, *ky*, zooms it in horizontal direction with the factor of *zoom_x* in vertical direction with the factor of *zoom_y*.  *flip_x* and *flip_y* state if the buffer shall be flipped (0 = no flipping, 1 = flipping). Alpha states the transparent color (BLIT_NO_ALPHA for no transparency)


# tile map

Tiles maps are used to build environments out of tiles (in order to save storage). Tile maps consist of two components: a tile map that states which tile has to be places where. And the tile data containing the actual image information. If a tile is repeated multiple times, then memory has been saved.


```
void blit_tile_map_mode7(coord_t kx,  // start in fb window x
					     coord_t ky,  // start in fb window y
					     coord_t w,   // window width
					     coord_t h,   // window height
					     float px,  // translation within window
					     float py,  // translation within window
					     float pz,  // translation within window
					     float pr,  // rotation within window
					     tile_map_t map_data,
					     tile_data_t tile_set,
					     gbuffer_t fb)
```

This blits a tile map in SNES-mode-7-style. A pseudo 3D affine transformation. Since the RP2040 interpolator does most of the work this works with a frame rate sufficient for full screen display.

*kx* , *ky* , w and h state the size of the image in the framebuffer. *px*, *py* and *pz* state the translation of the map (what part of the map you get to see) and *pr* states the rotation. See *Pico Racer* example.


```
void blit_tile_map_rot(coord_t kx,  // start in fb window x
					   coord_t ky,  // start in fb window y
					   coord_t w,   // window width
					   coord_t h,   // window height
					   coord_t px,  // translation within window
					   coord_t py,  
					   coord_t pivot_x,  // pivot point (in screen/buffer coords, 0/0 is upper left corner)
					   coord_t pivot_y,  
					   float rot,
					   float zoom_x,
					   float zoom_y,
					   tile_map_t map_data,
					   tile_data_t tile_set,
					   gbuffer_t buf)
```

This blits a tile map in top down style. Since the RP2040 interpolator does most of the work this works with a frame rate sufficient for full screen display. See *Pico Racer* example.

*kx* , *ky* , w and h state the size of the image in the (frame)buffer. *px* and *py*  state the translation of the map (what part of the map you get to see) and *pivot_x* and *pivot_y* state the coordinated of the pivot point in case you want to rotate the map by the angle *rot*. *zoom_x* and *zoom_y* state the zoom factor in horizontal resp. vertical direction.

