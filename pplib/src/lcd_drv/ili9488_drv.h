/*
 * based on TFT_eSPI by Bodmer (github.com/Bodmer)
 *
 * https://github.com/Bodmer/TFT_eSPI/blob/master/TFT_Drivers/ILI9341_Defines.h
 *
 * Please see Github site for license details.
 */
 
#ifndef ILI9488_DRV_H
#define ILI9488_DRV_H

/* ======================== includes ============================= */
#include <Arduino.h>
#include "../setup.h"

/* ======================== defines ============================= */
/* ---------------------- screen dimensions ----------------------*/
// physical values
//#define ILI9488_CROPPED
#ifdef ILI9488_CROPPED
#define PHYS_SCREEN_WIDTH   240 //320
#define PHYS_SCREEN_HEIGHT  320 //480
#else
#define PHYS_SCREEN_WIDTH   320
#define PHYS_SCREEN_HEIGHT  480
#endif

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

// Delay between some initialisation commands
#define ILI9488_INIT_DELAY 0x80 // Not used unless commandlist invoked

// Generic commands used by ILI9488_eSPI.cpp
#define ILI9488_NOP     0x00
#define ILI9488_SWRST   0x01

#define ILI9488_SLPIN   0x10
#define ILI9488_SLPOUT  0x11

#define ILI9488_INVOFF  0x20
#define ILI9488_INVON   0x21

#define ILI9488_DISPOFF 0x28
#define ILI9488_DISPON  0x29

#define ILI9488_CASET   0x2A
#define ILI9488_PASET   0x2B
#define ILI9488_RAMWR   0x2C

#define ILI9488_RAMRD   0x2E

#define ILI9488_MADCTL  0x36

#define ILI9488_MAD_MY  0x80
#define ILI9488_MAD_MX  0x40
#define ILI9488_MAD_MV  0x20
#define ILI9488_MAD_ML  0x10
#define ILI9488_MAD_RGB 0x00
#define ILI9488_MAD_BGR 0x08
#define ILI9488_MAD_MH  0x04
#define ILI9488_MAD_SS  0x02
#define ILI9488_MAD_GS  0x01

#define ILI9488_IDXRD   0x00 // ILI9341 only, indexed control register read

/* ==================== functions ==================== */
void lcd_controller_init();
void lcd_set_addr(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_enable_te();
void lcd_disable_te();
bool lcd_get_vblank();

#endif
