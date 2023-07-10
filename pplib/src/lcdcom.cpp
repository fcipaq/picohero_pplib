/*
 * pplib - a library for the Pico Hero handheld
 *
 * Copyright (C) 2023 Daniel Kammer (daniel.kammer@web.de)
 *
 * 8 bit DMA LUT implementation is based on ZuluSCSI's
 * github.com/ZuluSCSI/ZuluSCSI-firmware (see github page for liscense
 * details)
 *
 * lcd_send_dat_byte, lcd_send_cmd_byte, lcd_pio_wait (and probably more)
 * based on Bodmer's TFT_eSPI see https://github.com/Bodmer/TFT_eSPI/
 * for liscense details.
 *
 * see Github site for liscense details *
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

#include "lcdcom.h"
#include "gbuffers.h"
#include "colors.h"

#include "hardware/dma.h"
#include "hardware/pwm.h"

#if LCD_COLORDEPTH == 8
#include "lcd_pio_lut8.h"
#endif
#include "lcd_pio16.h"

/* ==================== mutex ==================== */
mutex_t lcd_scanout_complete;

/* ========================= variables ========================== */
/* ----------------------------- NN -----------------------------*/
bool lcd_init_complete = false;

/* ----------------------- backlight -----------------------------*/
pwm_config lcd_bl_pwm_cfg = pwm_get_default_config();

/* ----------------------- scanout -----------------------------*/
volatile uint16_t dma_scanline = 0;
volatile uint8_t scanline_even = 0;

#if LCD_COLORDEPTH == 8
volatile uint8_t* cur_scanout_buf = NULL;
#elif LCD_COLORDEPTH == 16
volatile uint16_t* cur_scanout_buf = NULL;
#endif

#if LCD_COLORDEPTH == 8 && !defined LCD_DOUBLE_PIXEL_LINEAR && defined LCD_DOUBLE_PIXEL_NEAREST
uint8_t scanline_buf[SCREEN_WIDTH * 2];
#elif LCD_COLORDEPTH == 16 && defined LCD_DOUBLE_PIXEL_NEAREST
uint16_t scanline_buf[SCREEN_WIDTH * 2];
#elif defined LCD_DOUBLE_PIXEL_LINEAR
uint16_t* scanline_buf_lst = (uint16_t*)malloc(SCREEN_WIDTH * 2 * 2);  // double resolution, 16 bpp
uint16_t* scanline_buf_nxt = (uint16_t*)malloc(SCREEN_WIDTH * 2 * 2);
uint16_t* scanline_buf_mid = (uint16_t*)malloc(SCREEN_WIDTH * 2 * 2);
#endif

/* ----------------------------- PIO -----------------------------*/
PIO lcd_pio = pio0;
int8_t lcd_pio_tft_sm = 0;
int8_t lcd_pio_lut_sm = 0;

// SM stalled mask
uint32_t pio_pull_stall_mask = 0;

// SM jump instructions to change SM behaviour
uint32_t pio_instr_jmp8 = 0;

/* ----------------------------- DMA -----------------------------*/
#if LCD_COLORDEPTH==8
#define LCD_NUM_DMA 3
#elif LCD_COLORDEPTH==16
#define LCD_NUM_DMA 1
#endif

int32_t lcd_dma_chan[LCD_NUM_DMA];
dma_channel_config lcd_dma_config[LCD_NUM_DMA];

uint8_t lcd_dma_enabled = 0;

/* -------------------- custom palette (LUT) ---------------------- */
#if LCD_COLORDEPTH==8
color_palette_t lcd_palette[256] __attribute__((aligned(512), section(".scratch_x.parity")));
#endif

/* ======================= implementation ======================== */

#if LCD_COLORDEPTH==8
color_palette_t* lcd_get_palette_ptr() {
  return &lcd_palette[0];
}

void lcd_std_palette() {
  // calculate standard color palette of 256 colors
  int rr, gg, bb;
  int i = 0;

  for (int r = 0; r < 8; r++)
    for (int g = 0; g < 8; g++)
      for (int b = 0; b < 4; b++) {
        rr = r * 5;
        if (rr > 31) rr = 31;

        gg = g * 9;

        bb = b * 11;
        if (bb > 31) bb = 31;

        //lcd_palette[i] = RGBColor565_565(rr, gg, bb);
        lcd_palette[i] = rr << 11 | gg << 5 | bb;

        i++;
      }
}
#endif

#if defined LCD_DOUBLE_PIXEL_NEAREST
void tx_scanline() {

  uint8_t corr = 0;

  if (scanline_even) {
    scanline_even = 0;
    dma_scanline++;
    // dma_scanline needs to be incremented before calling lcd_send_framebuffer to
    // prevent a race condition in the ISR. However, dma_scanline also needs to be
    // incremented after calling lcd_send_framebuffer for the buffer scanout to
    // work properly. Hence the "corr" approach.
    corr = 1;
  } else {
    scanline_even = 1;
  }

  lcd_send_framebuffer((void*)&cur_scanout_buf[(dma_scanline - corr) * SCREEN_WIDTH], SCREEN_WIDTH);

}  // txScanline_double_nearest 8 and 16 bits
#endif

#if defined LCD_DOUBLE_PIXEL_LINEAR
void tx_scanline() {
  if (scanline_even) {
    scanline_even = 0;
    dma_scanline++;
    lcd_send_framebuffer((void*)&scanline_buf_mid[0], SCREEN_WIDTH * 2);
    return;
  }

  // There is a bug here: dma_scanline is one ahead of the the scanline
  // calculation so that in the end the final scanline is displayed on top.

  uint16_t* scanline_buf_tmp = scanline_buf_lst;
  scanline_buf_lst = &scanline_buf_nxt[0];
  scanline_buf_nxt = scanline_buf_tmp;

  // this looks like a fine(er) job for the RP2040 interpolator

  if (dma_scanline < SCREEN_HEIGHT) {
    for (uint16_t h = 0; h < SCREEN_WIDTH * 2 - 1; h++) {
      if (h % 2 == 0)
        scanline_buf_nxt[h] = cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2];
      else {
        #if LCD_COLORDEPTH == 8
          uint8_t r1 = RGB565_red5(cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2]);
          uint8_t g1 = RGB565_green6(cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2]);
          uint8_t b1 = RGB565_blue5(cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2]);
          uint8_t r2 = RGB565_red5(cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2 + 1]);
          uint8_t g2 = RGB565_green6(cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2 + 1]);
          uint8_t b2 = RGB565_blue5(cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2 + 1]);
        #elif LCD_COLORDEPTH == 16
          uint8_t r1 = (cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2] >> 11) & 0x1f;
          uint8_t g1 = (cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2] >> 5) & 0x3f;
          uint8_t b1 = (cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2] & 0x1f);
          uint8_t r2 = (cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2 + 1] >> 11) & 0x1f;
          uint8_t g2 = (cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2 + 1] >> 5) & 0x3f;
          uint8_t b2 = (cur_scanout_buf[dma_scanline * SCREEN_WIDTH + h / 2 + 1] & 0x1f);
        #endif
        scanline_buf_nxt[h] = (r1 + r2) / 2 << 11 | (g1 + g2) / 2 << 5 | (b1 + b2) / 2;
      }
    }
  } else {
    for (uint16_t h = 0; h < SCREEN_WIDTH * 2; h++)
      scanline_buf_nxt[h] = 0;
  }

  if (dma_scanline != 0) {

    for (uint16_t h = 0; h < SCREEN_WIDTH * 2; h++) {
      uint8_t r1 = (scanline_buf_nxt[h] >> 11) & 0x1f;
      uint8_t g1 = (scanline_buf_nxt[h] >> 5) & 0x3f;
      uint8_t b1 = (scanline_buf_nxt[h] & 0x1f);
      uint8_t r2 = (scanline_buf_lst[h] >> 11) & 0x1f;
      uint8_t g2 = (scanline_buf_lst[h] >> 5) & 0x3f;
      uint8_t b2 = (scanline_buf_lst[h] & 0x1f);
      scanline_buf_mid[h] = (r1 + r2) / 2 << 11 | (g1 + g2) / 2 << 5 | (b1 + b2) / 2;
    }
  }

  scanline_even = 1;

  lcd_send_framebuffer((void*)&scanline_buf_lst[0], SCREEN_WIDTH * 2);

}  // txScanline_double_linear
#endif

void __isr lcd_dma_handler() {
  if (!dma_channel_get_irq0_status(lcd_dma_chan[0]))
    return;

  dma_channel_acknowledge_irq0(lcd_dma_chan[0]);  // clear irq flag

#if defined LCD_DOUBLE_PIXEL_LINEAR || defined LCD_DOUBLE_PIXEL_NEAREST
  if (dma_scanline == SCREEN_HEIGHT) {
    dma_scanline = 0;
    mutex_exit(&lcd_scanout_complete);
    return;
  }

  // In nearest neighbor mode there is virtually no performance gain when using core1
  // while linear fitting needs to be done with the interpolator anyways
  //multicore_reset_core1();
  //multicore_launch_core1(txScanline);
  tx_scanline();
#else
  // if scanout is performed in one chunk, we're done when the IRQ is called
  mutex_exit(&lcd_scanout_complete);
#endif

}  // lcd_dma_handler

bool lcd_dma_busy(void) {
  if (!lcd_dma_enabled)
    return false;

  if (dma_channel_is_busy(lcd_dma_chan[0]))
    return true;
  else
    return false;
}  // lcd_dma_busy

void lcd_dma_wait(void) {
  if (!lcd_dma_enabled)
    return;

  while (dma_channel_is_busy(lcd_dma_chan[0]))
    ;
}

volatile uint32_t output;

int lcd_dma_init() {
  if (lcd_dma_enabled)
    return LCD_SUCCESS;

  for (int h = 0; h < LCD_NUM_DMA; h++) {
    lcd_dma_chan[h] = dma_claim_unused_channel(true);

    if (lcd_dma_chan[h] < 0)
      return LCD_DMA_ERR;
  }

  for (int h = 0; h < LCD_NUM_DMA; h++)
    lcd_dma_config[h] = dma_channel_get_default_config(lcd_dma_chan[h]);

  // WARNING: if DMA size is changed here, transmission length in lcd_show_framebuffer/lcd_send_framebuffer must also be revised
  #if LCD_COLORDEPTH == 16
  channel_config_set_transfer_data_size(&lcd_dma_config[0], DMA_SIZE_16);
  channel_config_set_dreq(&lcd_dma_config[0], pio_get_dreq(lcd_pio, lcd_pio_tft_sm, true));
  channel_config_set_bswap(&lcd_dma_config[0], true);
  #elif LCD_COLORDEPTH == 8
  // Channel 0: user image buffer --> state machine
  channel_config_set_transfer_data_size(&lcd_dma_config[0], DMA_SIZE_8);
  channel_config_set_read_increment(&lcd_dma_config[0], true);
  channel_config_set_write_increment(&lcd_dma_config[0], false);
  channel_config_set_dreq(&lcd_dma_config[0], pio_get_dreq(lcd_pio, lcd_pio_lut_sm, true));

  // Channel 1: do the actual table lookup: state machine --> custom lut
  channel_config_set_transfer_data_size(&lcd_dma_config[1], DMA_SIZE_32);
  channel_config_set_read_increment(&lcd_dma_config[1], false);
  channel_config_set_write_increment(&lcd_dma_config[1], false);
  channel_config_set_dreq(&lcd_dma_config[1], pio_get_dreq(lcd_pio, lcd_pio_lut_sm, false));

  // Channel 2: the lookup is done, next send the acquired value to the ILI9342 SM: clut --> SM2
  channel_config_set_transfer_data_size(&lcd_dma_config[2], DMA_SIZE_16);
  channel_config_set_read_increment(&lcd_dma_config[2], false);
  channel_config_set_write_increment(&lcd_dma_config[2], false);
  channel_config_set_dreq(&lcd_dma_config[2], pio_get_dreq(lcd_pio, lcd_pio_tft_sm, true));
  channel_config_set_chain_to(&lcd_dma_config[2], lcd_dma_chan[1]);
  channel_config_set_bswap(&lcd_dma_config[2], true);

  dma_channel_configure(lcd_dma_chan[1],
                        &lcd_dma_config[1],
                        &dma_hw->ch[lcd_dma_chan[2]].al3_read_addr_trig,
                        &lcd_pio->rxf[lcd_pio_lut_sm],
                        1, true);

  dma_channel_configure(lcd_dma_chan[2],
                        &lcd_dma_config[2],
                        &lcd_pio->txf[lcd_pio_tft_sm],
                        NULL,
                        1, false);
  #endif

  // setup IRQ
  dma_channel_set_irq0_enabled(lcd_dma_chan[0], true);
  irq_set_exclusive_handler(DMA_IRQ_0, lcd_dma_handler);
  irq_set_enabled(DMA_IRQ_0, true);

  lcd_dma_enabled = true;

  return LCD_SUCCESS;
}  // dmaInit

void lcd_dma_shutdown(void) {
  if (!lcd_dma_enabled)
    return;

  dma_channel_unclaim(lcd_dma_chan[0]);

  lcd_dma_enabled = false;
}  // lcd_dma_shutdown

int lcd_check_ready() {

  // if mutex has no owen, then the tx is complete
  if (lcd_scanout_complete.owner == -1)
    return true;
  else
    return false;

}  // lcd_check_ready

void lcd_wait_ready() {
  while (!mutex_try_enter(&lcd_scanout_complete, NULL));
  mutex_exit(&lcd_scanout_complete);
}  // lcd_wait_ready

int lcd_show_framebuffer(gbuffer_t buf) {
  lcd_dma_wait();
  mutex_enter_blocking(&lcd_scanout_complete);

  cur_scanout_buf = buf.data;

  #if defined LCD_DOUBLE_PIXEL_LINEAR || defined LCD_DOUBLE_PIXEL_NEAREST
  //multicore_reset_core1();
  //multicore_launch_core1(txScanline);
  tx_scanline();
  return LCD_SUCCESS;
  #else
  // same length in both 16 and 8 bits --> different DMA_SIZE
  return lcd_send_framebuffer(buf.data, gbuf_get_width(buf) * gbuf_get_height(buf));
  #endif
}  // lcd_show_framebuffer

void lcd_set_speed(uint32_t freq) {
  //  if (!initComplete)
  //    return LCD_NOT_INIT;

  uint32_t fcpu = clock_get_hz(clk_sys);

  uint16_t clock_div = fcpu / freq;
  uint16_t fract_div = (uint16_t)(((float)fcpu / (float)freq - (float)clock_div) * 256.0);

  // max speed exceeded
  if (clock_div == 0) {
    clock_div = 1;
    fract_div = 0;
  }

  // Set clock divider and fractional divider
  pio_sm_set_enabled(lcd_pio, lcd_pio_tft_sm, false);
  pio_sm_set_clkdiv_int_frac(lcd_pio, lcd_pio_tft_sm, clock_div, fract_div);
  pio_sm_set_enabled(lcd_pio, lcd_pio_tft_sm, true);

  #if LCD_COLORDEPTH == 8
  pio_sm_set_enabled(lcd_pio, lcd_pio_lut_sm, false);
  pio_sm_set_clkdiv_int_frac(lcd_pio, lcd_pio_lut_sm, clock_div, fract_div);
  pio_sm_set_enabled(lcd_pio, lcd_pio_lut_sm, true);
  #endif
  
}  // lcd_set_speed

int lcd_pio_init() {

  lcd_pio_tft_sm = pio_claim_unused_sm(lcd_pio, false);

  if (lcd_pio_tft_sm < 0)
    return LCD_PIO_ERR;

  uint32_t program_offset = pio_add_program(lcd_pio, &lcd_output_program);

  pio_gpio_init(lcd_pio, PIN_LCD_WR);

  for (int i = 0; i < 8; i++)
    pio_gpio_init(lcd_pio, PIN_LCD_D0 + i);

  pio_sm_set_consecutive_pindirs(lcd_pio, lcd_pio_tft_sm, PIN_LCD_WR, 1, true);
  pio_sm_set_consecutive_pindirs(lcd_pio, lcd_pio_tft_sm, PIN_LCD_D0, 8, true);

  pio_sm_config c = lcd_output_program_get_default_config(program_offset);

  sm_config_set_sideset_pins(&c, PIN_LCD_WR);
  sm_config_set_out_pins(&c, PIN_LCD_D0, 8);
  sm_config_set_clkdiv_int_frac(&c, 10, 1);
  sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
  sm_config_set_out_shift(&c, true, true, 16);  // OSR shifts out to the right, autopull, pull threshold is 16 bits

  pio_sm_init(lcd_pio, lcd_pio_tft_sm, program_offset + lcd_output_offset_tx_dat, &c);  // ori
  pio_sm_set_enabled(lcd_pio, lcd_pio_tft_sm, true);

  pio_pull_stall_mask = 1u << (PIO_FDEBUG_TXSTALL_LSB + lcd_pio_tft_sm);  // ori
  pio_instr_jmp8 = pio_encode_jmp(program_offset + lcd_output_offset_tx_cmd);

#if LCD_COLORDEPTH == 8

  lcd_pio_lut_sm = pio_claim_unused_sm(lcd_pio, true);

  if (lcd_pio_lut_sm < 0)
    return LCD_PIO_ERR;

  program_offset = pio_add_program(lcd_pio, &calc_lut_addr_program);

  c = calc_lut_addr_program_get_default_config(program_offset);

  sm_config_set_out_shift(&c, true, false, 8);  // OSR shifts out to the right, autopull, pull threshold is 16 bits
  sm_config_set_in_shift(&c, false, false, 8);  // ISR shifts in the left, no auto push, push doesn't matter

  pio_sm_init(lcd_pio, lcd_pio_lut_sm, program_offset + calc_lut_addr_offset_calc_lut_entry, &c);  // ori

  // Load base address to state machine register x
  uint32_t addrbase = (uint32_t)&lcd_palette[0];

  assert((addrbase & 0x1FF) == 0);

  pio_sm_put(lcd_pio, lcd_pio_lut_sm, addrbase >> 9);
  pio_sm_exec(lcd_pio, lcd_pio_lut_sm, pio_encode_pull(false, false));
  pio_sm_exec(lcd_pio, lcd_pio_lut_sm, pio_encode_mov(pio_x, pio_osr));

  pio_sm_set_enabled(lcd_pio, lcd_pio_lut_sm, true);
#endif

  return LCD_SUCCESS;
}  // pioInit

void lcd_pio_wait() {
  // Wait for the PIO to stall (SM pull request finds no data in TX FIFO)
  // This is used to detect when the SM is idle and hence ready for a jump instruction
  lcd_pio->fdebug = pio_pull_stall_mask;
  while (!(lcd_pio->fdebug & pio_pull_stall_mask));
}  // lcd_pio_wait

void set_rs(byte value) {
  digitalWrite(PIN_LCD_DC, value);
}

void set_rst(byte value) {
  digitalWrite(PIN_LCD_RST, value);
}

void lcd_send_dat_byte(uint8_t dat) {
  // jump to pio_instr_jmp8
  lcd_pio->sm[lcd_pio_tft_sm].instr = pio_instr_jmp8;

  // write dat to SM FIFO
  lcd_pio->txf[lcd_pio_tft_sm] = dat;

  lcd_pio_wait();
}  // lcd_send_dat_byte

void lcd_send_cmd_byte(uint8_t cmd) {
  set_rs(LOW);

  // jump to pio_instr_jmp8
  lcd_pio->sm[lcd_pio_tft_sm].instr = pio_instr_jmp8;

  // write cmd to SM FIFO
  lcd_pio->txf[lcd_pio_tft_sm] = cmd;

  lcd_pio_wait();

  set_rs(HIGH);
}  // lcd_send_cmd_byte

uint32_t testy;

// send a buffer of DATA bytes to the display
int lcd_send_framebuffer(void* buf, uint32_t buffersize) {
  if ((buffersize == 0) || (!lcd_dma_enabled))
    return LCD_NOT_INIT;

  lcd_dma_wait();
  lcd_pio_wait();

#if LCD_COLORDEPTH == 8
  dma_channel_configure(lcd_dma_chan[0], &lcd_dma_config[0], &lcd_pio->txf[lcd_pio_lut_sm], buf, buffersize, true);
#elif LCD_COLORDEPTH == 16
  dma_channel_configure(lcd_dma_chan[0], &lcd_dma_config[0], &lcd_pio->txf[lcd_pio_tft_sm], buf, buffersize, true);
#endif

  return LCD_SUCCESS;
}  // lcd_send_framebuffer

void lcd_set_backlight(byte level) {
  pwm_set_gpio_level(PIN_LCD_BL_PWM, level);
}

int lcd_init() {
  if (lcd_init_complete)
    return LCD_SUCCESS;

  // configure pin modes
  pinMode(PIN_LCD_RST, OUTPUT);
  pinMode(PIN_LCD_DC, OUTPUT);

  if (lcd_pio_init() != LCD_SUCCESS)
    return LCD_PIO_ERR;

  lcd_set_speed(LCD_PIO_SPEED * 1000000);

  // LCD controller setup (hardware dependent)
  lcd_controller_init();

  if (lcd_dma_init() != LCD_SUCCESS)
    return LCD_DMA_ERR;

  mutex_init(&lcd_scanout_complete);

  // setup backlight PWM
  gpio_set_function(PIN_LCD_BL_PWM, GPIO_FUNC_PWM);
  uint slice_num_backlight = pwm_gpio_to_slice_num(PIN_LCD_BL_PWM);
  pwm_clear_irq(slice_num_backlight);
  pwm_config_set_clkdiv(&lcd_bl_pwm_cfg, 1000000.0f);
  pwm_config_set_wrap(&lcd_bl_pwm_cfg, 100);
  pwm_init(slice_num_backlight, &lcd_bl_pwm_cfg, true);

  //lcd_set_backlight(20);

#if LCD_COLORDEPTH == 8
  lcd_std_palette();
#endif

  lcd_init_complete = true;

  return LCD_SUCCESS;
}  // lcd_init
