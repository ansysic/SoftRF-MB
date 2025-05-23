/*
 * View_Time_EPD.cpp
 * Copyright (C) 2019-2022 Linar Yusupov
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

// this is modified from v1.1.

#include "../system/SoC.h"

#if defined(USE_EPAPER)

#include "../driver/EPD.h"
#include "../driver/Battery.h"
#include "BatteryIcon.h"
#include "../TrafficHelper.h"

#if defined(ARDUINO_ARCH_NRF52)
#include <pcf8563.h>
#include <bluefruit.h>

extern RTC_Date fw_build_date_time;
#endif /* ARDUINO_ARCH_NRF52 */

#include <gfxfont.h>
#include <FreeMonoBold24pt7b.h>
#include <FreeMonoBold12pt7b.h>
#include <FreeMono18pt7b.h>
#include "U8g2_for_Adafruit_GFX.h"

static const char TZ_text[] = "UTC";

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

static const uint8_t bt_icon[] = {
  0x0F, 0xF0, 0x1D, 0x38, 0x31, 0x98, 0x31, 0xCC,
  0x6D, 0xEC, 0x6F, 0x6C, 0x67, 0xC4, 0x63, 0x84,
  0x63, 0xC4, 0x67, 0xE4, 0x6D, 0x6C, 0x61, 0xEC,
  0x31, 0xCC, 0x31, 0x98, 0x1D, 0x38, 0x0F, 0xF0,
};

void EPD_time_setup()
{
  u8g2Fonts.begin(*display); // connect u8g2 procedures to Adafruit GFX
  BatteryIcon_setup(u8g2Fonts);
}

void EPD_time_loop()
{
  char buf_hm[16];
  char buf_sec[4];

  int16_t  tbx, tby;
  uint16_t tbw, tbh;

  if (isTimeToEPD()) {

#if defined(USE_EPD_TASK)
  if (EPD_update_in_progress == EPD_UPDATE_NONE) {
//  if (SoC->Display_lock()) {
#else
  {
#endif
    bool ble_has_client = false;

    strcpy(buf_hm, "--:--");
    strcpy(buf_sec, "  ");

#if defined(ARDUINO_ARCH_NRF52)

    if (rtc && rtc->isVaild()) {
      RTC_Date now = rtc->getDateTime();

      if (now.year >= fw_build_date_time.year &&
          now.year <  fw_build_date_time.year + 15) {

        snprintf(buf_hm,  sizeof(buf_hm),  "%2d:%02d", now.hour, now.minute);
        snprintf(buf_sec, sizeof(buf_sec), "%02d"    , now.second);
      }
    }

    ble_has_client = Bluefruit.connected();

#endif /* ARDUINO_ARCH_NRF52 */


    // "UTC"
    display->fillScreen(GxEPD_WHITE);
    display->setFont(&FreeMonoBold12pt7b);
    display->getTextBounds(TZ_text, 0, 0, &tbx, &tby, &tbw, &tbh);
    display->setCursor((display->width() - tbw) / 2, tbh + tbh / 2);
    display->print(TZ_text);

    // BT ICON
    if (ble_has_client) {
      display->drawBitmap(display->width() - 50, 6, bt_icon, 16, 16, GxEPD_BLACK);
    }

    // BATT ICON
    drawBatteryIcon(u8g2Fonts, display->width() -24 - 5, 5);

    // "hh:mm"
    display->setFont(&FreeMonoBold24pt7b);
    display->getTextBounds(buf_hm, 0, 0, &tbx, &tby, &tbw, &tbh);
    display->setCursor((display->width() - tbw) / 2, display->height() / 2 - 10);
    display->print(buf_hm);

    // "ss"
    display->setFont(&FreeMono18pt7b);
    display->getTextBounds(buf_sec, 0, 0, &tbx, &tby, &tbw, &tbh);
    display->setCursor((display->width() - tbw) / 2, display->height() / 2 + tbh + tbh - 20);
    display->print(buf_sec);

    // show the latest max RSSI
    int acrfts_counter = Traffic_Count();   // maxrssi is a byproduct
    if (maxrssi < 0) {
      snprintf(buf_hm,  sizeof(buf_hm),  "max RSSI %d", maxrssi);
      display->setFont(&FreeMonoBold12pt7b);
      display->getTextBounds(buf_hm, 0, 0, &tbx, &tby, &tbw, &tbh);
      display->setCursor((display->width() - tbw) / 2, display->height() - 20);
      display->print(buf_hm);
    }

#if defined(USE_EPD_TASK)
    /* a signal to background EPD update task */
    EPD_update_in_progress = EPD_UPDATE_FAST;
//    SoC->Display_unlock();
//    yield();
#else
    display->display(true);
#endif
  }
    EPDTimeMarker = millis();
  }
}

void EPD_time_next()
{

}

void EPD_time_prev()
{

}

#endif /* USE_EPAPER */
