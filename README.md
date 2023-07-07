# pplib - A library for the ~~Pico Pad~~ *Pico Hero* (an open source handheld based on the Raspberry Pi Pico) for Arduino

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
![Pico Hero picture 2](https://raw.githubusercontent.com/fcipaq/picohero_pplib/main/images/front3.jpg)

Front view

![Pico Hero picture 3](https://raw.githubusercontent.com/fcipaq/picohero_pplib/main/images/back.jpg)

Back view

![Pico Hero picture 4](https://raw.githubusercontent.com/fcipaq/picohero_pplib/main/images/top.jpg)


Top view

![Pico Hero picture 5](https://raw.githubusercontent.com/fcipaq/picohero_pplib/main/images/parts.jpg)

Parts (older version without SD card)

![Pico Hero picture 6](https://raw.githubusercontent.com/fcipaq/picohero_pplib/main/images/emulator.jpg)

*Pico Hero* running modified Sega Mega Drive emulator by bzhxx (https://github.com/bzhxx/gwenesis)

I will add more repositories, tools and details in time. Also, I am going to add Gerber files and schematics as well as STL files for printing/building the case. Consider this as preview. Please be patient, I am going at full speed.

## Specs:

- 3.2 inch LCD screen with 320x240 resolution with parallel interface to the RP2040
- analog control stick, 3 buttons and on/off switch
- 3W speaker
- micro SD card slot (for space constraints it’s push/pull so a it’s a little fiddly to remove the SD card) 
- size: 136mm x 60mm x 12mm; weight: 100g including 600 mAh battery
- optional charging circuitry when using a RPi Pico module without charger logic.

I originally intended to call the handheld *Pico Pad* – however a Czech company was a couple days faster.

Now, the device features a simple library which does the hardware handling. The library is for *Arduino* so you just need to put it in your libraries folder and can start developing right away.

The whole library is built around Earle Philhower's Raspberry Pi Pico Arduino core (no further dependencies are involve). 

## Core features:

- screen: 16 bit and 8 bit (must choose at compile time) buffers, full and half resolution panel fitter (nearest neighbor and experimental linear filter)
- fast blitter (using the RP2040's "interpolator") with zooming/rotation
- fast tilemap routines to build platformer, racing games, RPGs etc. (also using the RP2040's "interpolator")
- sound library for sound output (8 bit PCM up to 22000 Hz) with four channels for mixing
- font library with the ability to import true type fonts (FontEditor written by H. Reddmann)

## Requirements

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

# Installation

Place the *pplib* directory into your Arduino libraries folder. Next, install Earle Philhower's Raspberry Pi Pico Arduino core. The library is meant to be used with Arduino. It is written in non-object-oriented C++. At first you need to setup the hardware (16 bit or 8 bit color depth, resolution, panel fitter). See setup.h for details. Your sketch has then to call pplInit() right at the beginning. 
That’s it. You may now build the examples.

# Setup

Some compile time hardware setup can be done in setup.h

`LCD_PIO_SPEED`
 
 This compiler switch defines the interface speed to the ILI9341 controller in MHz. The speed calculation is with respect to CPU speed
 
`LCD_ROTATION`

States the screen orientation. Possible values are:

0 = 0° (portrait)
1 = 90° (landscape)
2 = 180° (portrait upside down)
3 = 270° (landscape upside down)

`LCD_COLORDEPTH`
`LCD_DOUBLE_PIXEL_LINEAR`  // works with 16 bit mode only
`LCD_DOUBLE_PIXEL_NEAREST`

# Library description:

## Types

// TODO


## Functions

### pplib

`int pplInit()`

This function is called to initialize all the hardware on the *Pico Hero*. It’s supposed to be called right at the beginning of `setup()`. This also does the bootloader handling (press all three buttons when powering up the *Pico Hero* to enter bootloader mode) – so it’s very important.
On success zero is returned otherwise an error code. If the LCD initialization fails, the bootloader is called.


### lcdcom

This is the hardware layer to interface with the LCD

`void lcd_set_speed(uint32_t freq)`
This sets the LCD interface speed to the ILI9341. Specs state a max frequency of approx. 30 MHz however 100 MHz should work fine. You should not call this in the middle of program execution (it’s called from `ppl_init()`). When the CPU is being overclocked, the function is aware of that and the PIO speed adjusted accordingly.

`int  lcd_show_framebuffer(gbuffer_t buf)`

Sends a graphics buffer to the LCD.

`void lcd_wait_ready()`

Waits until a pending buffer has been sent to the LCD.

`int  lcd_check_ready()`

Returns `true´ if the transmission subsystem is idle (i.e. is ready to send another buffer).

`void lcd_set_backlight(byte level)`

Sets the backlight level (from 0% to 100%)

`void lcd_enable_te()`

Enables the tearing signal.

`void lcd_disable_te()`

Disables the tearing signal.

`bool lcd_get_vblank()`

Returns the current state of the tearing signal. 


### gbuffers

All graphics are rendered into objects called graphics buffers (gbuffer_t). Those contain the information about the image size and color depth as well as a pointer to the actual image data.
The functions are overloaded to work with 8 bit as well as 16 bit color depths.
The library does not support rendering graphics (e.g. a line or a circle) directly to the screen.

`uint16_t  gbuf_get_width(gbuffer8_t buf)`

Returns the width of a buffer

`uint16_t  gbuf_get_height(gbuffer8_t buf)`

Returns the height of a buffer

`int gbuf_alloc(gbuffer8_t* buf, uint16_t width, uint16_t height, uint8_t colors) `

Allocates memory to an already existing graphics buffer object. Returns `BUF_ERR_NO_RAM` on failure otherwise `BUF_SUCCESS`

`uint8_t*  gbuf_get_dat_ptr(gbuffer8_t buf)`

Returns the data pointer of a buffer object.

`void gbuf_free(gbuffer8_t buf) `

Frees the data memory of a buffer object.


### blitter

The blitter blits a source buffer to a destination buffer. The buffers may be different in size but must be of the same color depth. There is only one (overloaded) function.

```
void blit_buf(coord_t kx,    // coordinates of upper left corner
              coord_t ky,    
              gbuffer_t src, // source buffer
              gbuffer_t dst, // destination buffer
              int32_t alpha)
```

Blits a buffer to another buffer at the position `kx`, `ky`. Alpha states the transparent color (BLIT_NO_ALPHA for no transparency)


```
void blit_buf(coord_t kx,    // x-coord where to blit the of CENTER of the image
              coord_t ky,    // y-coord where to blit the of CENTER of the image
              float zoom,    // zoom factor (same in both directions) 
              float rot,     // rotation of the image (in rad)
              int32_t alpha, // color which is NOT being drawn (BLIT_NO_ALPHA for no transparency)
              gbuffer_t src, // pointer to source buffer
              gbuffer_t dst) // pointer to destination buffer
```

Blits a buffer to another buffer at the position `kx`, `ky`, zooms it at the factor of `zoom` and rotates is at the angle of `rot`. Alpha states the transparent color (BLIT_NO_ALPHA for no transparency).
Note: Due to the nature of how the rotation is done with the "interpolator", when rotating the original image must be smaller in size than the buffer containing it. In fact it needs to be smaller by a factor of sqrt(2) x sqrt(1.75). This is because otherwise there will be artifacts from the "interpolator" folding back parts of the image into the visible area.
For example if the buffer is 16 by 8 pixels, the visible area must be centered and not exceed 8 by 4 pixels:

(X: allowed image area, O: blank area - background or "transparent color")

```
OOOOOOOOOOOOOOOO
OOOOOOOOOOOOOOOO
OOOOXXXXXXXXOOOO
OOOOXXXXXXXXOOOO
OOOOXXXXXXXXOOOO
OOOOXXXXXXXXOOOO
OOOOOOOOOOOOOOOO
OOOOOOOOOOOOOOOO
```

This is somewhat a tradeoff between memory usage and  performance. Images are therefore best stored in flash.
Both width and height of the image size must be power of 2.


```
void blit_buf(coord_t kx,     // x-coord where to blit the of CENTER of the image
              coord_t ky,     // y-coord where to blit the of CENTER of the image
              float zoom_x,   // zoom factor in x direction
              float zoom_y,   // zoom factor in y direction
              uint8_t flip_x, // whether to flip horizontally (1 means flip, 0 means
              uint8_t flip_y, // whether to flip vertically
              int32_t alpha,  // color which is NOT being drawn (BLIT_NO_ALPHA for no transparency)
              gbuffer_t src,  // pointer to source buffer
              gbuffer_t dst)  // pointer to destination buffer
```

Blits a buffer to another buffer at the position `kx`, `ky`, zooms it in horizontal direction with the factor of `zoom_x` in vertical direction with the factor of `zoom_y`.  `flip_x` and `flip_y` state if the buffer shall be flipped (0 = no flipping, 1 = flipping). Alpha states the transparent color (BLIT_NO_ALPHA for no transparency)
Since this operation is accelerated by the use of the "interpolator", both width and height of the image size must be power of 2.

### colors

Colors may be converted, composed or channels may be extracted using the following functions:

`color16_t rgb_col_888_565(uint8_t r, uint8_t g, uint8_t b)`

Returns the color in 16 bit machine readable format when given r g and b values in 24 bits. (R-G-B 8-8-8)
Ranges - r: 0..255, g: 0..255 and b: 0..255

`color8_t rgb_col_888_332(uint8_t r, uint8_t g, uint8_t b)`

Returns the color in 8 bit in machine readable format when given r g and b values in 24 bits. (R-G-B 8-8-8)
Ranges - r: 0..255, g: 0..255 and b: 0..255
   
`color16_t rgb_col_565_565(uint8_t r, uint8_t g, uint8_t b)`

Returns the color in 16 bit machine readable format when given r g and b values in 16 bits. R-G-B 5-6-5
Ranges - r: 0..31, g: 0..63 and b: 0..31
   
`color8_t  rgb_col_332_332(uint8_t r, uint8_t g, uint8_t b)`

Returns the color in 8 bit machine readable format when given r g and b values in 8 bits. (R-G-B 3-3-2)
Ranges - r: 0..7, g: 0..7 and b: 0..3

`uint8_t rgb_col_565_red(color16_t col)`

Returns the red component of a RGB565 color as a value ranging from 0..31

`uint8_t rgb_col_565_green(color16_t col)`

Returns the green component of a RGB565 color as a value ranging from 0..63

`uint8_t rgb_col_565_blue(color16_t col)`

Returns the blue component of a RGB565 color as a value ranging from 0..31

### fonts


### tile maps

Tiles maps are used to build environments out of tiles (in order to save storage). Tile maps consist of two components: a tile map that states which tile has to be placed where. And the tile data containing the actual image information. If a tile is repeated multiple times, then memory has been saved.

Tile map objects (tile_map_t) contain an 8 bit array and that means there is a maximum number of 256 different tiles per map allowed. Every byte of the map array holds an number that represents a certain tile. A tile data object (tile_data_t) is very similar to a graphics buffer but also contains the number of tiles stored which allows multiple tiles to be contained in a single object. The tiles themselves may be of 8 or 16 bit color depth. A constraint introduced by the way the "interpolator" handles the lookup is that the width and the height of a tile must be a power of 2 as does the width and the height of the map. For example a map may be 128 by 64 tiles. And each of the tiles may be of the size 64 by 32 pixels. All tiles of a tile data object are of the same size.


```
void tile_blit_mode7(coord_t kx,           // start in fb window x
                     coord_t ky,           // start in fb window y
                     coord_t w,            // window width
                     coord_t h,            // window height
                     float px,             // translation within window
                     float py,             // translation within window
                     float pz,             // translation within window
                     float pr,             // rotation within window
                     tile_map_t map_data,  // map data
                     tile_data_t tile_set, // tile data
                     int32_t alpha,        // transparency
                     gbuffer_t buf);       // pointer to destination buffer
```

This blits a tile map in SNES-mode-7-style. A pseudo 3D affine transformation. Since the RP2040 "interpolator" does most of the work this works with a frame rate sufficient for full screen display.

`kx` , `ky` , w and h state the size of the image in the framebuffer. `px`, `py` and `pz` state the translation of the map (what part of the map you get to see) and `pr` states the rotation. See *Pico Racer* example. `alpha` defines the color which is not being drawn (BLIT_NO_ALPHA for no transparency).


```
void tile_blit_rot(coord_t kx,            // start in fb window x
                   coord_t ky,            // start in fb window y
                   coord_t w,             // window width
                   coord_t h,             // window height
                   coord_t px,            // translation within window
                   coord_t py,  
                   coord_t pivot_x,       // pivot point (in screen/buffer coords, 0/0 is upper left corner)
                   coord_t pivot_y,  
                   float rot,             // angle
                   float zoom_x,          // zoom in horizontal direction
                   float zoom_y,          // zoom in vertical direction
                   tile_map_t map_data,   // map data
                   tile_data_t tile_set,  // tile data
                   int32_t alpha,         // transparency
                   gbuffer_t buf);        // pointer to destination buffer
```

This blits a tile map in top down style. Since the RP2040's "interpolator" does most of the work this works with a frame rate sufficient for full screen display. See *Pico Racer* example.

`kx` , `ky` , w and h state the size of the image in the (frame)buffer. `px` and `py`  state the translation of the map (what part of the map you get to see) and `pivot_x` and `pivot_y` state the coordinated of the pivot point in case you want to rotate the map by the angle `rot`. `zoom_x` and `zoom_y` state the zoom factor in horizontal resp. vertical direction. `alpha` defines the color which is not being drawn (BLIT_NO_ALPHA for no transparency).


### sound

This modules handles the playback of sounds. There are four sound channels which can be assigned buffers for playback. All currently playing buffers are mixed on the fly and sent via DMA to the PIO which then handles the PWM playback. The buffer format is a raw 8 bit PCM format. You need to specify the playback frequency using `snd_set_freq()`. All buffers play back with the same frequency and volume. The (master) volume can be set using `snd_set_vol(uint8_t vol)`. 

`int  snd_enque_buf(uint8_t *ext_buf, uint32_t buffersize, uint8_t num_snd_channel, bool blocking)`

Enques a buffer to be played. `ext_buf` is a pointer to an unsigned 8 bit array of a PCM buffer. You may state one of four channels (SND_CHAN_0 to SND_CHAN_3). If _blocking_ is set to `SND_BLOCKING` then the function will only return once the buffer is successfully enqueued (if there is no free buffer, the function will wait for a free buffer). If set to `SND_NONBLOCKING`, the function will immediately return even if the buffer was unable to be enqueued. If the buffer was successfully enqueued `SND_SUCCESS` is returned.
The buffer will be played with support of DMA and interrupts. So this is mostly fire-and-forget. Once the buffer is enqueued, it will be played without any further action needed.

`int  snd_num_bufs_free(uint8_t num_snd_channel)`

Returns the number of free buffers.

`int  snd_num_bufs_waiting(uint8_t num_snd_channel)`

Returns the number of buffers waiting (0 means idle)

`int  snd_cancel_channel(uint8_t num_snd_channel)`

Cancels playback on given channel (`SND_CHAN_ALL` cancels all playback)

`int  snd_set_freq(uint32_t freq)`

Sets the playback frequency.

`void snd_set_vol(uint8_t vol)`

Sets the volume level (from 0 to 5)
