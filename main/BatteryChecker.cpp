#include "BatteryChecker.h"
#include <Arduino.h>
#include "time_utils.h"

BatteryChecker::BatteryChecker(BatteryReader& batter_reader)
    : reader_(batter_reader),
    last_time_(0)
{}

void BatteryChecker::AddToRealTime(unsigned long time)
{
    last_time_ -= time;
}

bool BatteryChecker::Check()
{
    //unsigned long current_time = millis();
    unsigned long current_time = time();
    //if current_time will be < last_time_
    //then current_time - last_time must whatever get time distance
    if (0 == last_time_
        || current_time - last_time_ > s_to_time(TIME_BATTERY_CHECK))
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

uint32_t BatteryChecker::NextNeccessaryDiffTime(uint32_t current_time)
{
    return get_next_diff_time(last_time_, s_to_time(TIME_BATTERY_CHECK), current_time);
}