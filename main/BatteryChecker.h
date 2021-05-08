#pragma once

#include "BatteryReader.h"

#define TIME_BATTERY_CHECK 3600 * 2 * 1000L
#define MINIMUM_SEND_BATTERY 30 //%

class BatteryChecker
{
    public:
        
        explicit BatteryChecker(BatteryReader& batter_reader);

        bool Check();

        bool Update();

        //for sleep mode, must sub time from last time
        void AddToRealTime(unsigned long time);

        const char* GetData() const;

    private:

        BatteryReader& reader_;
        unsigned long last_time_;
};