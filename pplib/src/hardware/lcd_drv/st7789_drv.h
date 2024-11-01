/*
 * based on 
 *
 * 
 *
 * Please see Github site for license details.
 */

#ifndef ST7789_DRV_H
#define ST7789_DRV_H

/* ======================== includes ============================= */
#include <Arduino.h>
#include <setup.h>

/* ======================== defines ============================= */
/* ---------------------- screen dimensions ----------------------*/
// physical values
#define PHYS_SCREEN_WIDTH   240
#define PHYS_SCREEN_HEIGHT  320

/* --------------------- screen mode handling --------------------*/
#if defined LCD_DOUBLE_PIXEL_LINEAR || defined LCD_DOUBLE_PIXEL_NEAREST
  #if LCD_ROTATION==0 || LCD_ROTATION==2
    #define SCREEN_WIDTH   (PHYS_SCREEN_WIDTH / 2)
    #define SCREEN_HEIGHT  (PHYS_SCREEN_HEIGHT / 2)
  #elif LCD_ROTATION==1 || LCD_ROTATION==3
    #define SCREEN_WIDTH   (PHYS_SCREEN_HEIGHT / 2)
    #define SCREEN_HEIGHT  (PHYS_SCREEN_WIDTH / 2)
  #endif
#else
  #if LCD_ROTATION==0 || LCD_ROTATION==2
    #define SCREEN_WIDTH   PHYS_SCREEN_WIDTH
    #define SCREEN_HEIGHT  PHYS_SCREEN_HEIGHT
  #elif LCD_ROTATION==1 || LCD_ROTATION==3
    #define SCREEN_WIDTH   PHYS_SCREEN_HEIGHT
    #define SCREEN_HEIGHT  PHYS_SCREEN_WIDTH
  #endif
#endif

#define ST7789_RST_DELAY 120    ///< delay ms wait for reset finish
#define ST7789_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define ST7789_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define ST7789_NOP 0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID 0x04
#define ST7789_RDDST 0x09

#define ST7789_SLPIN 0x10
#define ST7789_SLPOUT 0x11
#define ST7789_PTLON 0x12
#define ST7789_NORON 0x13

#define ST7789_INVOFF 0x20
#define ST7789_INVON 0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON 0x29

#define ST7789_CASET 0x2A
#define ST7789_PASET 0x2B
#define ST7789_RAMWR 0x2C
#define ST7789_RAMRD 0x2E

#define ST7789_PTLAR 0x30
#define ST7789_COLMOD 0x3A
#define ST7789_MADCTL 0x36

#define ST7789_MADCTL_MY 0x80
#define ST7789_MADCTL_MX 0x40
#define ST7789_MADCTL_MV 0x20
#define ST7789_MADCTL_ML 0x10
#define ST7789_MADCTL_RGB 0x00

#define ST7789_RDID1 0xDA
#define ST7789_RDID2 0xDB
#define ST7789_RDID3 0xDC
#define ST7789_RDID4 0xDD

#define ST7789_RAMCTRL 0xB0
#define ST7789_PORCTRL 0xB2
#define ST7789_GCTRL 0xB7

#define ST7789_VCOMS 0xbb

#define ST7789_PVGAMCTRL 0xe0
#define ST7789_NVGAMCTRL 0xe1

/* ==================== functions ==================== */
void lcd_controller_init();
void lcd_set_addr(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_enable_te();
void lcd_disable_te();
bool lcd_get_vblank();

#endif
