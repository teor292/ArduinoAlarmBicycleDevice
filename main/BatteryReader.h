#pragma once

#include "SafeString.h"
#include <Stream.h>
#include "BlockTimeReader.h"

class BatteryReader
{
    public:

        explicit BatteryReader(SafeString& tmp_string, Stream& serial, BlockTimeReader &reader);

        bool ReadBattery();

        const char * GetData() const;
        unsigned char GetDigitValue() const;

    private:
        
        SafeString& tmp_string_;
        Stream& serial_;
        BlockTimeReader& reader_;
        char battery_result_[21];
        unsigned char digit_value_;
};