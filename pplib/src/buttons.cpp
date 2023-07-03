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

#pragma GCC optimize("Ofast")

#include "buttons.h"
#include "setup.h"

uint16_t x_0 = 400;
uint16_t y_0 = 400;
uint16_t max_x = 600;
uint16_t min_x = 200;
uint16_t max_y = 600;
uint16_t min_y = 200;

int lst_ret_x;
int lst_ret_y;

// Values under this are considered as zero
#define zeroThreshold 25   // 5^2
// Decrement the max. amplitude by this value, so that 100% can be reached
#define minmaxTolerance 10

void recalibAnalog() {
  x_0 = analogRead(PIN_ANALOG_X);
  y_0 = analogRead(PIN_ANALOG_Y);
  max_x = x_0 + 200;
  min_x = x_0 - 200;
  max_y = y_0 + 200;
  min_y = y_0 - 200;
}

int8_t getAnalogX() {
  int8_t ret;
  uint16_t cur_x = analogRead(PIN_ANALOG_X);

  if (cur_x > max_x)
    max_x = cur_x - minmaxTolerance;

  if (cur_x < min_x)
    min_x = cur_x + minmaxTolerance;

  if (cur_x > x_0)    
    ret = (cur_x - x_0) * 100 / (max_x - x_0);
  else
    ret = (cur_x - x_0) * 100 / (x_0 - min_x);

   // if |ret| < zeroThreshold^2 return zero
  if (ret * ret < zeroThreshold)
   ret = 0;
  
  // if the new value differs less than 10% from the old one it's being discarded
  if ((ret - lst_ret_x) * (ret - lst_ret_x) > zeroThreshold*zeroThreshold) {
    lst_ret_x = ret;
  } else {
    ret = lst_ret_x;
  }

  if (ret < -100)
    ret = -100;
  if (ret > 100)
    ret = 100;

  return (ret / 10) * -10;
}

int8_t getAnalogY() {
  int8_t ret;
  uint16_t cur_y = analogRead(PIN_ANALOG_Y);

  if (cur_y > max_y)
    max_y = cur_y - minmaxTolerance;

  if (cur_y < min_y)
    min_y = cur_y + minmaxTolerance;

  if (cur_y > y_0)    
    ret = (cur_y - y_0) * 100 / (max_y - y_0);
  else
    ret = (cur_y - y_0) * 100 / (y_0 - min_y);

  // if |ret| < zeroThreshold^2 return zero
  if (ret * ret < zeroThreshold)
   ret = 0;

  // if the new value differs less than 10% from the old one it's being discarded
  if ((ret - lst_ret_y) * (ret - lst_ret_y) > zeroThreshold*zeroThreshold) {
    lst_ret_y = ret;
  } else {
    ret = lst_ret_y;
  }

  if (ret < -100)
    ret = -100;
  if (ret > 100)
    ret = 100;

  return (ret / 10) * 10;
}

uint16_t getDPad() {
  uint16_t cur_x = analogRead(PIN_ANALOG_X);
  uint16_t cur_y = analogRead(PIN_ANALOG_Y);

  uint16_t value = 0;

  if (cur_x > max_x)
    max_x = cur_x;

  if (cur_y > max_y)
    max_y = cur_y;

  if (cur_x < min_x)
    min_x = cur_x;

  if (cur_y < min_y)
    min_y = cur_y; 

  if (cur_x > 650)
    value |= DPAD_LEFT;

  if (cur_x < 300)
    value |= DPAD_RIGHT;

  if (cur_y < 300)
    value |= DPAD_UP;

  if (cur_y > 650)
    value |= DPAD_DOWN;

  return value;
}

uint16_t getButtons() {
  uint16_t value = 0;

  if (digitalRead(PIN_BUTTON_1) == BUTTON_PRESSED)
    value |= BUTTON_1;

  if (digitalRead(PIN_BUTTON_2) == BUTTON_PRESSED)
    value |= BUTTON_2;

  if (digitalRead(PIN_BUTTON_3) == BUTTON_PRESSED)
    value |= BUTTON_3;

  #ifdef PIN_BUTTON_4
    if (digitalRead(PIN_BUTTON_4) == BUTTON_PRESSED)
      value |= BUTTON_4;
  #endif

  return value;
}

int ctrl_init() {
  pinMode(PIN_ANALOG_X, INPUT);
  pinMode(PIN_ANALOG_Y, INPUT);

  recalibAnalog();

  pinMode(PIN_BUTTON_1, BUTTON_PULL_MODE);
  pinMode(PIN_BUTTON_2, BUTTON_PULL_MODE);
  pinMode(PIN_BUTTON_3, BUTTON_PULL_MODE);
  #ifdef PIN_BUTTON_4
  pinMode(PIN_BUTTON_4, BUTTON_PULL_MODE);
  #endif
  
  return 0;
}
