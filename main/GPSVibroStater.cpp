#include "GPSVibroStater.h"

#if defined(GPS)

void GPSVibroStater::Alarm(bool alarm)
{
    if (!enabled_) return;
    is_alarm_ |= alarm;
}

void GPSVibroStater::EnableAlarm(bool enable)
{
    enabled_ = enable;
    is_alarm_ = false;
}

bool GPSVibroStater::IsAlarm()
{
    if (!enabled_) return false;
    if (!is_alarm_) return false;
    is_alarm_ = false;
    return true;
}

#endif