/*
 * BatteryIcon.cpp
 * Copyright (C) 2019-2025 Linar Yusupov
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
 */

#include "../system/SoC.h"

#if defined(USE_EPAPER)

#include "BatteryIcon.h"
#include "../driver/Battery.h"

void BatteryIcon_setup(U8G2_FOR_ADAFRUIT_GFX &u8g2) {
  // Set up the battery icon font
  u8g2.setFontDirection(1);
  u8g2.setForegroundColor(GxEPD_BLACK);
  u8g2.setBackgroundColor(GxEPD_WHITE);
  u8g2.setFont(u8g2_font_battery24_tr);
}

void drawBatteryIcon(U8G2_FOR_ADAFRUIT_GFX &u8g2, uint16_t x, uint16_t y) {
  // Draw the battery icon at the specified position
  u8g2.setCursor(x, y);
  uint8_t pct = Battery_charge();
  uint8_t bars = pct / 20;
  if (bars > 4) bars = 4;
  u8g2.write('0' + bars);
}

#endif /* USE_EPAPER */
