/*
 * based on TFT_eSPI by Bodmer (github.com/Bodmer)
 *
 * https://github.com/Bodmer/TFT_eSPI/blob/master/TFT_Drivers/ILI9341_Defines.h
 *
 * Please see Github site for license details.
 */

#include "../hwcfg.h"

#ifdef LCD_DRIVER_ILI9488

#include <Arduino.h>
#include "ili9488_drv.h"

/* ==================== forward declarations ==================== */
void lcd_send_dat_byte(uint8_t cmd);
void lcd_send_cmd_byte(uint8_t cmd);
void set_rs(byte value);
void set_rst(byte value);

/* ==================== functions ==================== */
void lcd_enable_te() {
  //  lcdSendCmdByte(ILI9341_TEON);
  //  lcdSendDatByte(0x00); // V-blank info only
}

void lcd_disable_te() {
  //  lcd_send_cmd_byte(ILI9341_TEOFF);
}

bool lcd_get_vblank() {
  //return digitalRead(LCD_PIN_TE);
  return false;
}

void lcd_set_addr(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {

  uint8_t coordinates[8] = {
    (uint8_t)(x1 >> 8),
    (uint8_t)(x1 & 0xff),
    (uint8_t)(x2 >> 8),
    (uint8_t)(x2 & 0xff),
    (uint8_t)(y1 >> 8),
    (uint8_t)(y1 & 0xff),
    (uint8_t)(y2 >> 8),
    (uint8_t)(y2 & 0xff)
  };

  lcd_send_cmd_byte(ILI9488_CASET);  // column address set
  for (int i = 0; i < 4; i++)
    lcd_send_dat_byte(coordinates[i]);

  lcd_send_cmd_byte(ILI9488_PASET);  // page address set
  for (int i = 0; i < 4; i++)
    lcd_send_dat_byte(coordinates[i + 4]);

  lcd_send_cmd_byte(ILI9488_RAMWR);  //memory write

  // WA for a PIO program bug that causes the state transistion
  // being too ealry before the pins have been written (?)
  #if !defined LCD_DOUBLE_PIXEL_NEAREST
    for (int i = 0; i < 2; i++)
      lcd_send_dat_byte(0x00);
  #else
    for (int i = 0; i < 4; i++)
      lcd_send_dat_byte(0x00);
  #endif
}

void lcd_controller_init() {
  set_rst(HIGH);
  delay(20);
  set_rst(LOW);
  delay(20);
  set_rst(HIGH);

  delay(50);

  // taken from ILI9488_eSPI
  // https://github.com/Bodmer/ILI9488_eSPI/
  // This is the command sequence that initialises the ILI9488 driver


  // Configure ILI9488 display

  lcd_send_cmd_byte(0xE0);  // Positive Gamma Control
  lcd_send_dat_byte(0x00);
  lcd_send_dat_byte(0x03);
  lcd_send_dat_byte(0x09);
  lcd_send_dat_byte(0x08);
  lcd_send_dat_byte(0x16);
  lcd_send_dat_byte(0x0A);
  lcd_send_dat_byte(0x3F);
  lcd_send_dat_byte(0x78);
  lcd_send_dat_byte(0x4C);
  lcd_send_dat_byte(0x09);
  lcd_send_dat_byte(0x0A);
  lcd_send_dat_byte(0x08);
  lcd_send_dat_byte(0x16);
  lcd_send_dat_byte(0x1A);
  lcd_send_dat_byte(0x0F);

  lcd_send_cmd_byte(0XE1);  // Negative Gamma Control
  lcd_send_dat_byte(0x00);
  lcd_send_dat_byte(0x16);
  lcd_send_dat_byte(0x19);
  lcd_send_dat_byte(0x03);
  lcd_send_dat_byte(0x0F);
  lcd_send_dat_byte(0x05);
  lcd_send_dat_byte(0x32);
  lcd_send_dat_byte(0x45);
  lcd_send_dat_byte(0x46);
  lcd_send_dat_byte(0x04);
  lcd_send_dat_byte(0x0E);
  lcd_send_dat_byte(0x0D);
  lcd_send_dat_byte(0x35);
  lcd_send_dat_byte(0x37);
  lcd_send_dat_byte(0x0F);

  lcd_send_cmd_byte(0XC0);  // Power Control 1
  lcd_send_dat_byte(0x17);
  lcd_send_dat_byte(0x15);

  lcd_send_cmd_byte(0xC1);  // Power Control 2
  lcd_send_dat_byte(0x41);

  lcd_send_cmd_byte(0xC5);  // VCOM Control
  lcd_send_dat_byte(0x00);
  lcd_send_dat_byte(0x12);
  lcd_send_dat_byte(0x80);

  lcd_send_cmd_byte(ILI9488_MADCTL);  // Memory Access Control
  lcd_send_dat_byte(0x48);            // MX, BGR

  lcd_send_cmd_byte(0x3A);  // Pixel Interface Format
  lcd_send_dat_byte(0x55);  // 16 bit colour for parallel

  lcd_send_cmd_byte(0xB0);  // Interface Mode Control
  lcd_send_dat_byte(0x00);

  lcd_send_cmd_byte(0xB1);  // Frame Rate Control
  lcd_send_dat_byte(0xA0);

  lcd_send_cmd_byte(0xB4);  // Display Inversion Control
  lcd_send_dat_byte(0x02);

  lcd_send_cmd_byte(0xB6);  // Display Function Control
  lcd_send_dat_byte(0x02);
  lcd_send_dat_byte(0x02);
  lcd_send_dat_byte(0x3B);

  lcd_send_cmd_byte(0xB7);  // Entry Mode Set
  lcd_send_dat_byte(0xC6);

  lcd_send_cmd_byte(0xF7);  // Adjust Control 3
  lcd_send_dat_byte(0xA9);
  lcd_send_dat_byte(0x51);
  lcd_send_dat_byte(0x2C);
  lcd_send_dat_byte(0x82);

  lcd_send_cmd_byte(ILI9488_SLPOUT);  //Exit Sleep
  delay(120);

  lcd_send_cmd_byte(ILI9488_DISPON);  //Display on
  delay(25);

  // End of ILI9488 display configuration

  lcd_send_cmd_byte(0x35);  // tearing
  lcd_send_dat_byte(0x0);

  lcd_send_cmd_byte(ILI9488_RAMWR);  // Write GRAM
  for (uint32_t jj = 0; jj < 320 * 480 * 2; jj++)
    lcd_send_dat_byte(random(256));

  lcd_send_cmd_byte(ILI9488_MADCTL);  // Memory Access Control

  #ifdef ILI9488_CROPPED
    #if LCD_ROTATION == 0
      lcd_send_dat_byte(ILI9488_MAD_MX | ILI9488_MAD_BGR);  // Rotation 0 (portrait mode)
      lcd_set_addr(160 - PHYS_SCREEN_WIDTH / 2, 240 - PHYS_SCREEN_HEIGHT / 2, 160 + PHYS_SCREEN_WIDTH / 2 - 1, 240 + PHYS_SCREEN_HEIGHT / 2 - 1);
    #elif LCD_ROTATION == 1
      lcd_send_dat_byte(ILI9488_MAD_MV | ILI9488_MAD_MX | ILI9488_MAD_MY | ILI9488_MAD_BGR);  // Rotation 90 (landscape mode)
      lcd_set_addr(240 - PHYS_SCREEN_HEIGHT / 2, 160 - PHYS_SCREEN_WIDTH / 2, 240 + PHYS_SCREEN_HEIGHT / 2 - 1, 160 + PHYS_SCREEN_WIDTH / 2 - 1);
    #elif LCD_ROTATION == 2
      lcd_send_dat_byte(ILI9488_MAD_MY | ILI9488_MAD_BGR);  // Rotation 180 (portrait mode)
      lcd_set_addr(160 - PHYS_SCREEN_WIDTH / 2, 240 - PHYS_SCREEN_HEIGHT / 2, 160 + PHYS_SCREEN_WIDTH / 2 - 1, 240 + PHYS_SCREEN_HEIGHT / 2 - 1);
    #elif LCD_ROTATION == 3
      lcd_send_dat_byte(ILI9488_MAD_MV | ILI9488_MAD_BGR);  // Rotation 270 (landscape mode)
      lcd_set_addr(240 - PHYS_SCREEN_HEIGHT / 2, 160 - PHYS_SCREEN_WIDTH / 2, 240 + PHYS_SCREEN_HEIGHT / 2 - 1, 160 + PHYS_SCREEN_WIDTH / 2 - 1);
    #endif
  #else
    #if LCD_ROTATION == 0
      lcd_send_dat_byte(ILI9488_MAD_MX | ILI9488_MAD_BGR);  // Rotation 0 (portrait mode)
      lcd_set_addr(0, 0, PHYS_SCREEN_WIDTH - 1, PHYS_SCREEN_HEIGHT - 1);
    #elif LCD_ROTATION == 1
      lcd_send_dat_byte(ILI9488_MAD_MV | ILI9488_MAD_MX | ILI9488_MAD_MY | ILI9488_MAD_BGR);  // Rotation 90 (landscape mode)
      lcd_set_addr(0, 0, PHYS_SCREEN_HEIGHT - 1, PHYS_SCREEN_WIDTH - 1);
    #elif LCD_ROTATION == 2
      lcd_send_dat_byte(ILI9488_MAD_MY | ILI9488_MAD_BGR);  // Rotation 180 (portrait mode)
      lcd_set_addr(0, 0, PHYS_SCREEN_WIDTH - 1, PHYS_SCREEN_HEIGHT - 1);
    #elif LCD_ROTATION == 3
      lcd_send_dat_byte(ILI9488_MAD_MV | ILI9488_MAD_BGR);  // Rotation 270 (landscape mode)
      lcd_set_addr(0, 0, PHYS_SCREEN_HEIGHT - 1, PHYS_SCREEN_WIDTH - 1);
    #endif
  #endif

  lcd_send_cmd_byte(ILI9488_RAMWR);  // Write GRAM
}

#endif