#include "BatteryChecker.h"
#include <Arduino.h>

BatteryChecker::BatteryChecker(BatteryReader& batter_reader)
    : reader_(batter_reader),
    last_time_(0)
{}

bool BatteryChecker::Check()
{
    unsigned long current_time = millis();
    if (0 == last_time_
        || last_time_ > current_time 
        || current_time - last_time_ > TIME_BATTERY_CHECK)
    {
        last_time_ = current_time;
        return true;
    }
    return false;
}

bool BatteryChecker::Update()
{
    if (reader_.ReadBattery())
    {
        auto value = reader_.GetDigitValue();
        if (value <= MINIMUM_SEND_BATTERY) return true;
    }
    return false;
}

const char * BatteryChecker::GetData() const
{
    return reader_.GetData();
}