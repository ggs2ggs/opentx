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

#ifndef _OPENTX_CONSTANTS_H_
#define _OPENTX_CONSTANTS_H_

enum SwitchConfig {
  SWITCH_NONE,
  SWITCH_TOGGLE,
  SWITCH_2POS,
  SWITCH_3POS,
};

enum PotConfig {
  POT_NONE,
  POT_WITH_DETENT,
  POT_MULTIPOS_SWITCH,
  POT_WITHOUT_DETENT
};

enum SliderConfig {
  SLIDER_NONE,
  SLIDER_WITH_DETENT,
};

enum fsStartPositionType {
  FS_START_UP,
  FS_START_DOWN,
  FS_START_PREVIOUS
};

enum CalibrationState {
  CALIB_START = 0,
#if defined(RADIO_CALIBRATION_HALL)
  CALIB_SET_P0,
  CALIB_SET_P1,
  CALIB_SET_P2,
  CALIB_SET_P3,
  CALIB_SET_P4,
  CALIB_SET_P5,
  CALIB_SET_P6,
  CALIB_SET_P7,
  CALIB_SET_P8,
  CALIB_CAL_POINTS,
  CALIB_SET_STICKS_RANGE,
#endif
  CALIB_SET_MIDPOINT,
  CALIB_MOVE_STICKS,
  CALIB_STORE,
  CALIB_FINISHED
};

#endif // _OPENTX_CONSTANTS_H_
