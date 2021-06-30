#pragma once

#include "BatteryReader.h"
#include "AbstractNextAwakeTimeGetter.h"

#define TIME_BATTERY_CHECK 3600 * 2UL
#define MINIMUM_SEND_BATTERY 30 //%

class BatteryChecker : public AbstractNextAwakeTimeGetter
{
    public:
        
        explicit BatteryChecker(BatteryReader& batter_reader);

        bool Check();

        bool Update();

        //for sleep mode, must sub time from last time
        void AddToRealTime(unsigned long time);

        const char* GetData() const;

        uint32_t NextNeccessaryDiffTime(uint32_t current_time) override;

    private:

        BatteryReader& reader_;
        unsigned long last_time_;
};