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


void onBindMenu(const char * result)
{
  uint8_t moduleIdx = CURRENT_MODULE_EDITED(menuVerticalPosition - HEADER_LINE);

  if (result == STR_BINDING_1_8_TELEM_ON) {
    if(isModuleMultimodule(moduleIdx)) {
      g_model.moduleData[moduleIdx].multi.receiverTelemetryOff = false;
      g_model.moduleData[moduleIdx].multi.receiverHigherChannels = false;
    }
    else {
      g_model.moduleData[moduleIdx].pxx.receiverTelemetryOff = false;
      g_model.moduleData[moduleIdx].pxx.receiverHigherChannels = false;
    }
  }
  else if (result == STR_BINDING_1_8_TELEM_OFF) {
    if(isModuleMultimodule(moduleIdx)) {
      g_model.moduleData[moduleIdx].multi.receiverTelemetryOff = true;
      g_model.moduleData[moduleIdx].multi.receiverHigherChannels = false;
    }
    else {
      g_model.moduleData[moduleIdx].pxx.receiverTelemetryOff = true;
      g_model.moduleData[moduleIdx].pxx.receiverHigherChannels = false;
    }
  }
  else if (result == STR_BINDING_9_16_TELEM_ON) {
    if(isModuleMultimodule(moduleIdx)) {
      g_model.moduleData[moduleIdx].multi.receiverTelemetryOff = false;
      g_model.moduleData[moduleIdx].multi.receiverHigherChannels = true;
    }
    else {
      g_model.moduleData[moduleIdx].pxx.receiverTelemetryOff = false;
      g_model.moduleData[moduleIdx].pxx.receiverHigherChannels = true;
    }
  }
  else if (result == STR_BINDING_9_16_TELEM_OFF) {
      if(isModuleMultimodule(moduleIdx)) {
        g_model.moduleData[moduleIdx].multi.receiverTelemetryOff = true;
        g_model.moduleData[moduleIdx].multi.receiverHigherChannels = true;
      }
      else {
        g_model.moduleData[moduleIdx].pxx.receiverTelemetryOff = true;
        g_model.moduleData[moduleIdx].pxx.receiverHigherChannels = true;
      }
  }
  else {
    return;
  }

  moduleState[moduleIdx].mode = MODULE_MODE_BIND;
}

void startBindMenu(uint8_t moduleIdx)
{
  uint8_t selection = 0; // R9M_LBT should default to 0 as available options are variables

  if (isTelemAllowedOnBind(moduleIdx))
    POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_ON);
  POPUP_MENU_ADD_ITEM(STR_BINDING_1_8_TELEM_OFF);

  if (isBindCh9To16Allowed(moduleIdx)) {
    if (isTelemAllowedOnBind(moduleIdx))
      POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_ON);
    POPUP_MENU_ADD_ITEM(STR_BINDING_9_16_TELEM_OFF);
  }

  if (isBindCh9To16Allowed(moduleIdx) && g_model.moduleData[moduleIdx].pxx.receiverHigherChannels) {
    selection += 1;
    if (isTelemAllowedOnBind(moduleIdx))
      selection += 1;
  }

  if (isTelemAllowedOnBind(moduleIdx) && g_model.moduleData[moduleIdx].pxx.receiverTelemetryOff)
    selection += 1;

  POPUP_MENU_SELECT_ITEM(selection);
  POPUP_MENU_START(onBindMenu);
}
