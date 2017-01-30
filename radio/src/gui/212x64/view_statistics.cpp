/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"

#define STATS_1ST_COLUMN               FW/2
#define STATS_2ND_COLUMN               12*FW+FW/2
#define STATS_3RD_COLUMN               24*FW+FW/2
#define STATS_LABEL_WIDTH              4*FW

void menuStatisticsView(event_t event)
{
  TITLE(STR_MENUSTAT);

  switch(event)
  {
    case EVT_KEY_FIRST(KEY_UP):
      chainMenu(menuStatisticsDebug);
      break;

    case EVT_KEY_LONG(KEY_MENU):
      g_eeGeneral.globalTimer = 0;
      storageDirty(EE_GENERAL);
      sessionTimer = 0;
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      break;
  }

  // Session and Total timers
  lcdDrawText(STATS_1ST_COLUMN, FH*1+1, "SES", BOLD);
  drawTimer(STATS_1ST_COLUMN + STATS_LABEL_WIDTH, FH*1+1, sessionTimer, 0, 0);
  lcdDrawText(STATS_1ST_COLUMN, FH*2+1, "TOT", BOLD);
  drawTimer(STATS_1ST_COLUMN + STATS_LABEL_WIDTH, FH*2+1, g_eeGeneral.globalTimer + sessionTimer, TIMEHOUR, 0);

  // Throttle special timers
  lcdDrawText(STATS_2ND_COLUMN, FH*0+1, "THR", BOLD);
  drawTimer(STATS_2ND_COLUMN + STATS_LABEL_WIDTH, FH*0+1, s_timeCumThr, 0, 0);
  lcdDrawText(STATS_2ND_COLUMN, FH*1+1, "TH%", BOLD);
  drawTimer(STATS_2ND_COLUMN + STATS_LABEL_WIDTH, FH*1+1, s_timeCum16ThrP/16, 0, 0);

  // Timers
  for (int i=0; i<TIMERS; i++) {
    drawStringWithIndex(STATS_3RD_COLUMN, FH*i+1, "TM", i+1, BOLD);
    if (timersStates[i].val > 3600)
      drawTimer(STATS_3RD_COLUMN + STATS_LABEL_WIDTH, FH*i+1, timersStates[i].val, TIMEHOUR, 0);
    else
      drawTimer(STATS_3RD_COLUMN + STATS_LABEL_WIDTH, FH*i+1, timersStates[i].val, 0, 0);
  }

#if defined(THRTRACE)
  const coord_t x = 5;
  const coord_t y = 60;
  lcdDrawSolidHorizontalLine(x-3, y, MAXTRACE+3+3);
  lcdDrawSolidVerticalLine(x, y-32, 32+3);
  for (coord_t i=0; i<MAXTRACE; i+=6) {
    lcdDrawSolidVerticalLine(x+i+6, y-1, 3);
  }

  uint16_t traceRd = s_traceWr > MAXTRACE ? s_traceWr - MAXTRACE : 0;
  for (coord_t i=1; i<=MAXTRACE && traceRd<s_traceWr; i++, traceRd++) {
    uint8_t h = s_traceBuf[traceRd % MAXTRACE];
    lcdDrawSolidVerticalLine(x+i, y-h, h);
  }
#endif
}

#define MENU_DEBUG_COL1_OFS   (11*FW-2)
#define MENU_DEBUG_Y_MIXMAX   (2*FH-3)
#define MENU_DEBUG_Y_LUA      (3*FH-2)
#define MENU_DEBUG_Y_FREE_RAM (4*FH-1)
#define MENU_DEBUG_Y_USB      (5*FH)
#define MENU_DEBUG_Y_RTOS     (6*FH)

#if defined(USB_SERIAL)
  extern uint16_t usbWraps;
  extern uint16_t charsWritten;
  extern "C" volatile uint32_t APP_Rx_ptr_in;
  extern "C" volatile uint32_t APP_Rx_ptr_out;
#endif

void menuStatisticsDebug(event_t event)
{
  TITLE(STR_MENUDEBUG);

#if defined(WATCHDOG_TEST)
  if (warningResult) {
    warningResult = 0;
    // do a user requested watchdog test
    TRACE("Performing watchdog test");
    pausePulses();
  }
#endif

  switch(event)
  {
    case EVT_KEY_LONG(KEY_ENTER):
      g_eeGeneral.globalTimer = 0;
      storageDirty(EE_GENERAL);
      sessionTimer = 0;
      killEvents(event);
      break;
    case EVT_KEY_FIRST(KEY_ENTER):
#if defined(LUA)
      maxLuaInterval = 0;
      maxLuaDuration = 0;
#endif
      maxMixerDuration  = 0;
      break;

#if defined(DEBUG_TRACE_BUFFER)
    case EVT_KEY_FIRST(KEY_UP):
      pushMenu(menuTraceBuffer);
      return;
#endif

    case EVT_KEY_FIRST(KEY_DOWN):
      chainMenu(menuStatisticsView);
      break;
    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      break;
#if defined(WATCHDOG_TEST)
    case EVT_KEY_LONG(KEY_MENU):
      {
        POPUP_CONFIRMATION("Test the watchdog?");
        const char * w = "The radio will reset!";
        SET_WARNING_INFO(w, strlen(w), 0);
      }
      break;
#endif
  }

  lcdDrawTextAlignedLeft(MENU_DEBUG_Y_FREE_RAM, "Free Mem");
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_FREE_RAM, availableMemory(), LEFT);
  lcdDrawText(lcdRightPos, MENU_DEBUG_Y_FREE_RAM, "b");

#if defined(LUA)
  lcdDrawTextAlignedLeft(MENU_DEBUG_Y_LUA, "Lua scripts");
  lcdDrawText(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_LUA+1, "[Duration]", SMLSIZE);
  lcdDrawNumber(lcdRightPos, MENU_DEBUG_Y_LUA, 10*maxLuaDuration, LEFT);
  lcdDrawText(lcdRightPos+2, MENU_DEBUG_Y_LUA+1, "[Interval]", SMLSIZE);
  lcdDrawNumber(lcdRightPos, MENU_DEBUG_Y_LUA, 10*maxLuaInterval, LEFT);
#endif

  lcdDrawTextAlignedLeft(MENU_DEBUG_Y_MIXMAX, STR_TMIXMAXMS);
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_MIXMAX, DURATION_MS_PREC2(maxMixerDuration), PREC2|LEFT);
  lcdDrawText(lcdRightPos, MENU_DEBUG_Y_MIXMAX, "ms");

#if !defined(SIMU) && defined(USB_SERIAL)
  lcdDrawTextAlignedLeft(MENU_DEBUG_Y_USB, "Usb");
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_USB, charsWritten, LEFT);
  lcdDrawText(lcdRightPos, MENU_DEBUG_Y_USB, " ");
  lcdDrawNumber(lcdRightPos, MENU_DEBUG_Y_USB, APP_Rx_ptr_in, LEFT);
  lcdDrawText(lcdRightPos, MENU_DEBUG_Y_USB, " ");
  lcdDrawNumber(lcdRightPos, MENU_DEBUG_Y_USB, APP_Rx_ptr_out, LEFT);
  lcdDrawText(lcdRightPos, MENU_DEBUG_Y_USB, " ");
  lcdDrawNumber(lcdRightPos, MENU_DEBUG_Y_USB, usbWraps, LEFT);
#endif

  lcdDrawTextAlignedLeft(MENU_DEBUG_Y_RTOS, STR_FREESTACKMINB);
  lcdDrawText(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_RTOS+1, "[M]", SMLSIZE);
  lcdDrawNumber(lcdRightPos, MENU_DEBUG_Y_RTOS, menusStack.available(), LEFT);
  lcdDrawText(lcdRightPos+2, MENU_DEBUG_Y_RTOS+1, "[X]", SMLSIZE);
  lcdDrawNumber(lcdRightPos, MENU_DEBUG_Y_RTOS, mixerStack.available(), LEFT);
  lcdDrawText(lcdRightPos+2, MENU_DEBUG_Y_RTOS+1, "[A]", SMLSIZE);
  lcdDrawNumber(lcdRightPos, MENU_DEBUG_Y_RTOS, audioStack.available(), LEFT);
  lcdDrawText(lcdRightPos+2, MENU_DEBUG_Y_RTOS+1, "[I]", SMLSIZE);
  lcdDrawNumber(lcdRightPos, MENU_DEBUG_Y_RTOS, stackAvailable(), LEFT);

  lcdDrawText(3*FW, 7*FH+1, STR_MENUTORESET);
  lcdInvertLastLine();
}


#if defined(DEBUG_TRACE_BUFFER)
void menuTraceBuffer(event_t event)
{
  switch(event)
  {
    case EVT_KEY_LONG(KEY_ENTER):
      dumpTraceBuffer();
      killEvents(event);
      break;
  }

  SIMPLE_SUBMENU("Trace Buffer", TRACE_BUFFER_LEN);
  /* RTC time */
  drawRtcTime(LCD_W+1, 0, LEFT|TIMEBLINK);

  uint8_t y = 0;
  uint8_t k = 0;
  int8_t sub = menuVerticalPosition;

  lcdDrawChar(0, FH, '#');
  lcdDrawText(4*FW, FH, "Time");
  lcdDrawText(14*FW, FH, "Event");
  lcdDrawText(20*FW, FH, "Data");

  for (uint8_t i=0; i<LCD_LINES-2; i++) {
    y = 1 + (i+2)*FH;
    k = i+menuVerticalOffset;

    //item
    lcdDrawNumber(0, y, k, LEFT | (sub==k ? INVERS : 0));

    const struct TraceElement * te = getTraceElement(k);
    if (te) {
      //time
      putstime_t tme = te->time % SECS_PER_DAY;
      drawTimer(4*FW, y, tme, TIMEHOUR|LEFT, TIMEHOUR|LEFT);
      //event
      lcdDrawNumber(14*FW, y, te->event, LEADING0|LEFT, 3);
      //data
      lcdDrawSizedText  (20*FW, y, "0x", 2);
      lcdDrawHexNumber(22*FW-2, y, (uint16_t)(te->data >> 16));
      lcdDrawHexNumber(25*FW, y, (uint16_t)(te->data & 0xFFFF));
    }

  }


}
#endif //#if defined(DEBUG_TRACE_BUFFER)
