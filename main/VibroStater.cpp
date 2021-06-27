#include "VibroStater.h"
#include "header.h"
#include <Arduino.h>

#define ALARM_TIME 2 * 60 * 1000L //1 sms per two minutes

VibroStater::VibroStater(VibroAlarmChangeCallback callback)
    : 
    alarm_callback_(callback)
{
}

void VibroStater::Alarm(bool alarm) 
{
    if (!enabled_) return;
    is_alarm_ |= alarm;
}

bool VibroStater::Update()
{
    if (!enabled_) return false;

    if (!is_alarm_) return false;
    is_alarm_ = false;

    auto current_time = millis();

    if (current_time > last_alarm_time_ + ALARM_TIME
            || 0 == last_alarm_time_)
    {
        last_alarm_time_ = current_time;
        return true;
    }
    return false;

}

void VibroStater::EnableAlarm(bool enable)
{
    if (enabled_ != enable)
    {
        alarm_callback_(enable);
    }
    enabled_ = enable;
    last_alarm_time_ = 0;
    is_alarm_ = false;
}