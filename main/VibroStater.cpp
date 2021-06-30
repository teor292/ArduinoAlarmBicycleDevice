#include "VibroStater.h"
#include "header.h"
#include <Arduino.h>
#include "time_utils.h"

#define ALARM_TIME 2 * 60UL //1 sms per two minutes

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

    auto current_time = time();
    if (0 == last_alarm_time_
        || current_time -  last_alarm_time_ > s_to_time(ALARM_TIME))
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