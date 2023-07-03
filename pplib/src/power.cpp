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

#include "power.h"
#include "setup.h"

uint8_t lst_bat_lvl = 100;

// Return supply voltage in mV
uint16_t pwr_get_sply_vol() {
  return analogRead(PIN_BAT_ADC) * 10;  // voltage divider returns voltage in cV
}

// Return type of currently used power source:
// either on battery or on grid
int pwr_get_source() {
  if (pwr_get_sply_vol() > PWR_EXT_MIN_VOLT) {
    lst_bat_lvl = 100;
    return PWR_ON_GRD;
  } else {
    return PWR_ON_BAT;
  }
}

#if 0
// status display wear leveling
/*
 * battery OCV [mV] vs. SoC [%] LUT
 * OCV can be measured typically up to a current of 25 mA (see data sheet below)
 * src: https://pdfserv.maximintegrated.com/en/an/AN4189.pdf
 */
const unsigned int bat_chg_by_vol[] PROGMEM =
{ 
  3306, //   0%
  3687, //  10%
  3741, //  20%
  3775, //  30%
  3793, //  40%
  3821, //  50%
  3884, //  60%
  3945, //  70%
  4008, //  80%
  4086, //  90%
  4177  // 100%
};
#endif

// Return battery power level in percent
int pwr_get_bat_level() {
  if (pwr_get_source() == PWR_ON_GRD)
    return PWR_BAT_LVL_UNKOWN;
  
  uint8_t level;

  #if 0
  for (level = 0; level < 10; level++) {
    if (bat_chg_by_vol[level] > pwr_get_sply_vol() )
      break;
  }
  #else
  level = (pwr_get_sply_vol() - PWR_BAT_MIN_VOLT) * 10 / (PWR_BAT_MAX_VOLT - PWR_BAT_MIN_VOLT);
  #endif

  // return level in steps of 10, because the voltage divider
  // will otherwise deliver very inaccurate data
  level *= 10;
    
  if (level > 100)
    level = 100;

  if (level < 0)
    level = 0;

  #if 0
  if (level < lst_bat_lvl)
    lst_bat_lvl = level;

  return lst_bat_lvl < level ? lst_bat_lvl : level;
  #else
  return level;
  #endif

}

int pwr_init()
{
  pinMode(PIN_BAT_ADC, INPUT);
  
  return true;
}
