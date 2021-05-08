#pragma once

#include <avr/eeprom.h>

struct Settings
{
    unsigned char alarm;

    void Save()
    {
         eeprom_update_block(this, 0, sizeof(*this));
    }

    void Load()
    {
          eeprom_read_block(this, 0, sizeof(*this));
    }
};