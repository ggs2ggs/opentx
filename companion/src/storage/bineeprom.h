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

#ifndef _BINEEPROM_H_
#define _BINEEPROM_H_

#include "storage.h"

class BinEepromFormat : public StorageFormat
{
  public:
    BinEepromFormat(const QString & filename):
      StorageFormat(filename)
    {
    }
    
    virtual bool load(RadioData & radioData);
    virtual bool write(const RadioData & radioData);
    
  protected:
    bool extract(RadioData & radioData, const QByteArray & eeprom);
    virtual bool writeToFile(const uint8_t * eeprom, uint32_t size);
};

#endif // _BINEEPROM_H_
