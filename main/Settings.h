#pragma once

#if !defined(__SAMD21G18A__)
#include <avr/eeprom.h>
#endif

struct Settings
{
    unsigned char alarm{0};

    void Save()
    {
        #if !defined(__SAMD21G18A__)
         eeprom_update_block(this, 0, sizeof(*this));
        #endif
    }

    void Load()
    {
        #if !defined(__SAMD21G18A__)
          eeprom_read_block(this, 0, sizeof(*this));
        #endif
    }
};