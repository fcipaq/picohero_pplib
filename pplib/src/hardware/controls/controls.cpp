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

#pragma GCC optimize("Ofast")

#include "controls.h"
#include <hwcfg.h>

uint16_t _analog_x_0 = 400;
uint16_t _analog_y_0 = 400;
uint16_t _analog_max_x = 600;
uint16_t _analog_min_x = 200;
uint16_t _analog_max_y = 600;
uint16_t _analog_min_y = 200;

int _ctrl_lst_ret_x;
int _ctrl_lst_ret_y;

// Values below this threshold are considered as zero
#define zeroThreshold 25   // 5^2
// Decrement the max. amplitude by this value, so that 100% can be reached
#define minmaxTolerance 10

void ctrl_recalib_analog() {
  _analog_x_0 = analogRead(PIN_ANALOG_X);
  _analog_y_0 = analogRead(PIN_ANALOG_Y);
  _analog_max_x = _analog_x_0 + 200;
  _analog_min_x = _analog_x_0 - 200;
  _analog_max_y = _analog_y_0 + 200;
  _analog_min_y = _analog_y_0 - 200;
}

int8_t ctrl_analog_x_state() {
  int8_t ret;
  uint16_t cur_x = analogRead(PIN_ANALOG_X);

  if (cur_x > _analog_max_x)
    _analog_max_x = cur_x - minmaxTolerance;

  if (cur_x < _analog_min_x)
    _analog_min_x = cur_x + minmaxTolerance;

  if (cur_x > _analog_x_0)    
    ret = (cur_x - _analog_x_0) * 100 / (_analog_max_x - _analog_x_0);
  else
    ret = (cur_x - _analog_x_0) * 100 / (_analog_x_0 - _analog_min_x);

   // if |ret| < zeroThreshold^2 return zero
  if (ret * ret < zeroThreshold)
   ret = 0;
  
  // if the new value differs less than 10% from the old one it's being discarded
  if ((ret - _ctrl_lst_ret_x) * (ret - _ctrl_lst_ret_x) > zeroThreshold*zeroThreshold) {
    _ctrl_lst_ret_x = ret;
  } else {
    ret = _ctrl_lst_ret_x;
  }

  if (ret < -100)
    ret = -100;
  if (ret > 100)
    ret = 100;

  return (ret / 10) * -10;
}

int8_t ctrl_analog_y_state() {
  int8_t ret;
  uint16_t cur_y = analogRead(PIN_ANALOG_Y);

  if (cur_y > _analog_max_y)
    _analog_max_y = cur_y - minmaxTolerance;

  if (cur_y < _analog_min_y)
    _analog_min_y = cur_y + minmaxTolerance;

  if (cur_y > _analog_y_0)    
    ret = (cur_y - _analog_y_0) * 100 / (_analog_max_y - _analog_y_0);
  else
    ret = (cur_y - _analog_y_0) * 100 / (_analog_y_0 - _analog_min_y);

  // if |ret| < zeroThreshold^2 return zero
  if (ret * ret < zeroThreshold)
   ret = 0;

  // if the new value differs less than 10% from the old one it's being discarded
  if ((ret - _ctrl_lst_ret_y) * (ret - _ctrl_lst_ret_y) > zeroThreshold*zeroThreshold) {
    _ctrl_lst_ret_y = ret;
  } else {
    ret = _ctrl_lst_ret_y;
  }

  if (ret < -100)
    ret = -100;
  if (ret > 100)
    ret = 100;

  return (ret / 10) * 10;
}

uint16_t ctrl_dpad_state() {
  uint16_t cur_x = analogRead(PIN_ANALOG_X);
  uint16_t cur_y = analogRead(PIN_ANALOG_Y);

  uint16_t value = 0;

  if (cur_x > _analog_max_x)
    _analog_max_x = cur_x;

  if (cur_y > _analog_max_y)
    _analog_max_y = cur_y;

  if (cur_x < _analog_min_x)
    _analog_min_x = cur_x;

  if (cur_y < _analog_min_y)
    _analog_min_y = cur_y; 

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

uint16_t ctrl_button_state() {
  uint16_t value = 0;

  if (digitalRead(PIN_BUTTON_1) == BUTTON_PRESSED)
    value |= BUTTON_1;

  if (digitalRead(PIN_BUTTON_2) == BUTTON_PRESSED)
    value |= BUTTON_2;

  if (digitalRead(PIN_BUTTON_3) == BUTTON_PRESSED)
    value |= BUTTON_3;

  return value;
}

int ctrl_init() {
  pinMode(PIN_ANALOG_X, INPUT);
  pinMode(PIN_ANALOG_Y, INPUT);

  ctrl_recalib_analog();

  pinMode(PIN_BUTTON_1, BUTTON_PULL_MODE);
  pinMode(PIN_BUTTON_2, BUTTON_PULL_MODE);
  pinMode(PIN_BUTTON_3, BUTTON_PULL_MODE);

  return 0;
}
