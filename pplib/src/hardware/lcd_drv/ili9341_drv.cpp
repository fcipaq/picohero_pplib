/*
 * based on TFT_eSPI by Bodmer (github.com/Bodmer)
 *
 * https://github.com/Bodmer/TFT_eSPI/blob/master/TFT_Drivers/ILI9341_Defines.h
 *
 * Please see Github site for license details.
 */

#include <hwcfg.h>

#ifdef LCD_DRIVER_ILI9341

#include <Arduino.h>
#include "ili9341_drv.h"

/* ==================== forward declarations ==================== */
void lcd_send_dat_byte(uint8_t cmd);
void lcd_send_cmd_byte(uint8_t cmd);
void set_rs(byte value);
void set_rst(byte value);

/* ==================== functions ==================== */
void lcd_enable_te() {
  lcd_send_cmd_byte(ILI9341_TEON);
  lcd_send_dat_byte(0x00); // V-blank info only
}

void lcd_disable_te() {
  lcd_send_cmd_byte(ILI9341_TEOFF);
}  

bool lcd_get_vblank() {
  return digitalRead(PIN_LCD_TE);
}

void lcd_set_addr(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
 
  uint8_t coordinates[8] = {
    (uint8_t) (x1 >> 8),
    (uint8_t) (x1 & 0xff),
    (uint8_t) (x2 >> 8),
    (uint8_t) (x2 & 0xff),
    (uint8_t) (y1 >> 8),
    (uint8_t) (y1 & 0xff),
    (uint8_t) (y2 >> 8),
    (uint8_t) (y2 & 0xff)
  };
  
  lcd_send_cmd_byte(ILI9341_CASET); // column address set
  for (int i = 0; i < 4; i++)
    lcd_send_dat_byte(coordinates[i]);

    
  lcd_send_cmd_byte(ILI9341_PASET);  // page address set
  for (int i = 0; i < 4; i++)
    lcd_send_dat_byte(coordinates[i + 4]);

  lcd_send_cmd_byte(ILI9341_RAMWR); //memory write 

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
  pinMode(PIN_LCD_TE, INPUT);

  set_rst(HIGH);
  delay(20);
  set_rst(LOW);
  delay(20);
  set_rst(HIGH);

  delay(50);

  lcd_send_cmd_byte(0xEF);
  lcd_send_dat_byte(0x03);
  lcd_send_dat_byte(0x80);
  lcd_send_dat_byte(0x02);

  lcd_send_cmd_byte(0xCF);
  lcd_send_dat_byte(0x00);
  lcd_send_dat_byte(0XC1);
  lcd_send_dat_byte(0X30);

  lcd_send_cmd_byte(0xED);
  lcd_send_dat_byte(0x64);
  lcd_send_dat_byte(0x03);
  lcd_send_dat_byte(0X12);
  lcd_send_dat_byte(0X81);

  lcd_send_cmd_byte(0xE8);
  lcd_send_dat_byte(0x85);
  lcd_send_dat_byte(0x00);
  lcd_send_dat_byte(0x78);

  lcd_send_cmd_byte(0xCB);
  lcd_send_dat_byte(0x39);
  lcd_send_dat_byte(0x2C);
  lcd_send_dat_byte(0x00);
  lcd_send_dat_byte(0x34);
  lcd_send_dat_byte(0x02);

  lcd_send_cmd_byte(0xF7);
  lcd_send_dat_byte(0x20);

  lcd_send_cmd_byte(0xEA);
  lcd_send_dat_byte(0x00);
  lcd_send_dat_byte(0x00);

/*
  lcd_send_cmd_byte(ILI9341_PWCTR1);    //Power control
  lcd_send_dat_byte(0x23);   //VRH[5:0]

  lcd_send_cmd_byte(ILI9341_PWCTR2);    //Power control
  lcd_send_dat_byte(0x10);   //SAP[2:0];BT[3:0]

  lcd_send_cmd_byte(ILI9341_VMCTR1);    //VCM control
  lcd_send_dat_byte(0x3e);
  lcd_send_dat_byte(0x28);
*/

  lcd_send_cmd_byte(ILI9341_PWCTR1);    //Power control
  lcd_send_dat_byte(0x0b);   // 3.4V reference
  
  lcd_send_cmd_byte(ILI9341_PWCTR2);    //Power control
  lcd_send_dat_byte(0x10);   //SAP[2:0];BT[3:0]

  lcd_send_cmd_byte(ILI9341_VMCTR1);    //VCM control
  lcd_send_dat_byte(0x1c);  // 3.4V VCOMH
  lcd_send_dat_byte(0x28);  // -0.5V VCOML
  
  lcd_send_cmd_byte(ILI9341_VMCTR2);    //VCM control2
  lcd_send_dat_byte(0x86);  //--

  lcd_send_cmd_byte(ILI9341_PIXFMT);
  lcd_send_dat_byte(0x55);

  lcd_send_cmd_byte(ILI9341_FRMCTR1);
  lcd_send_dat_byte(0x00);
  lcd_send_dat_byte(0x13); // 0x18 79Hz, 0x1B default 70Hz, 0x13 100Hz

  lcd_send_cmd_byte(ILI9341_DFUNCTR);    // Display Function Control
  lcd_send_dat_byte(0x08);
  lcd_send_dat_byte(0x82);
  lcd_send_dat_byte(0x27);

  lcd_send_cmd_byte(0xF2);    // 3Gamma Function Disable
  lcd_send_dat_byte(0x00);

  lcd_send_cmd_byte(ILI9341_GAMMASET);    //Gamma curve selected
  lcd_send_dat_byte(0x01);

  lcd_send_cmd_byte(ILI9341_GMCTRP1);    //Set Gamma
  lcd_send_dat_byte(0x0F);
  lcd_send_dat_byte(0x31);
  lcd_send_dat_byte(0x2B);
  lcd_send_dat_byte(0x0C);
  lcd_send_dat_byte(0x0E);
  lcd_send_dat_byte(0x08);
  lcd_send_dat_byte(0x4E);
  lcd_send_dat_byte(0xF1);
  lcd_send_dat_byte(0x37);
  lcd_send_dat_byte(0x07);
  lcd_send_dat_byte(0x10);
  lcd_send_dat_byte(0x03);
  lcd_send_dat_byte(0x0E);
  lcd_send_dat_byte(0x09);
  lcd_send_dat_byte(0x00);

  lcd_send_cmd_byte(ILI9341_GMCTRN1);    //Set Gamma
  lcd_send_dat_byte(0x00);
  lcd_send_dat_byte(0x0E);
  lcd_send_dat_byte(0x14);
  lcd_send_dat_byte(0x03);
  lcd_send_dat_byte(0x11);
  lcd_send_dat_byte(0x07);
  lcd_send_dat_byte(0x31);
  lcd_send_dat_byte(0xC1);
  lcd_send_dat_byte(0x48);
  lcd_send_dat_byte(0x08);
  lcd_send_dat_byte(0x0F);
  lcd_send_dat_byte(0x0C);
  lcd_send_dat_byte(0x31);
  lcd_send_dat_byte(0x36);
  lcd_send_dat_byte(0x0F);

  lcd_send_cmd_byte(ILI9341_SLPOUT);    //Exit Sleep
 
  set_rs(LOW);
  delay(120);
  set_rs(HIGH);
  
  lcd_enable_te();
  
  lcd_send_cmd_byte(ILI9341_DISPON);    //Display on

  lcd_send_cmd_byte(ILI9341_MADCTL);    // Memory Access Control
  
  #if LCD_ROTATION==0
    lcd_send_dat_byte(ILI9341_MAD_MX | ILI9341_MAD_COLOR_ORDER); // Rotation 0 (portrait mode)
 	lcd_set_addr(0, 0, PHYS_SCREEN_WIDTH - 1, PHYS_SCREEN_HEIGHT - 1);
  #elif LCD_ROTATION==1
    lcd_send_dat_byte(ILI9341_MAD_MV | ILI9341_MAD_MX | ILI9341_MAD_MY | ILI9341_MAD_COLOR_ORDER); // Rotation 90 (landscape mode)
	lcd_set_addr(0, 0, PHYS_SCREEN_HEIGHT - 1, PHYS_SCREEN_WIDTH - 1);
  #elif LCD_ROTATION==2
    lcd_send_dat_byte(ILI9341_MAD_MY | ILI9341_MAD_COLOR_ORDER); // Rotation 180 (portrait mode)
 	lcd_set_addr(0, 0, PHYS_SCREEN_WIDTH - 1, PHYS_SCREEN_HEIGHT - 1);
  #elif LCD_ROTATION==3
    lcd_send_dat_byte(ILI9341_MAD_MV | ILI9341_MAD_COLOR_ORDER); // Rotation 270 (landscape mode)
	lcd_set_addr(0, 0, PHYS_SCREEN_HEIGHT - 1, PHYS_SCREEN_WIDTH - 1);
  #endif

  // clear screen to black
  for (uint32_t jj = 0; jj < PHYS_SCREEN_WIDTH * PHYS_SCREEN_HEIGHT * 2; jj++)
    lcd_send_dat_byte(0x00);

}

#endif