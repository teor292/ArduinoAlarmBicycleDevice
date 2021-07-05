#include "GPSIntervalWaiter.h"
#include "time_utils.h"

void GPSIntervalWaiter::SetSettings(const SendSettingData& settings)
{
    current_set_time_ = time();
    interval_delay_ = s_to_time(settings.interval_delay_time);
    interval_wait_ = s_to_time(settings.interval_not_send_time);
    wait_ = false;
}

GPS_INTERVAL_STATUS GPSIntervalWaiter::Update(uint32_t current_time)
{
    if (0 == interval_wait_) return GPS_INTERVAL_STATUS::NON_WAIT;
    if (wait_) return wait_update_(current_time);
    return non_wait_update_(current_time);
}

GPS_INTERVAL_STATUS GPSIntervalWaiter::non_wait_update_(uint32_t current_time)
{
    auto next_time = current_set_time_ + interval_delay_;
    //if overflow
    if (next_time < current_set_time_)
    {
        //if overflow
        if (current_time < current_set_time_)
        {
            //if 2 overflows -> simple compare
            if (next_time <= current_time)
            {
                wait_ = true;
                return GPS_INTERVAL_STATUS::TO_WAIT;
            }
            return GPS_INTERVAL_STATUS::NON_WAIT;
        }
        //if one overflow -> current_time is smaller next time
        return GPS_INTERVAL_STATUS::NON_WAIT;
    }
    //if overflow only current_time
    if (current_time < current_set_time_
        || next_time <= current_time)
    {
        wait_ = true;
        return GPS_INTERVAL_STATUS::TO_WAIT;
    }
    return GPS_INTERVAL_STATUS::NON_WAIT;
}

GPS_INTERVAL_STATUS GPSIntervalWaiter::wait_update_(uint32_t current_time)
{
    auto next_time = current_set_time_ + interval_delay_ + interval_wait_;
    //if overflow
    if (next_time < current_set_time_)
    {
        //if overflow
        if (current_time < current_set_time_)
        {
            //if 2 overflows -> simple compare
            if (next_time <= current_time)
            {
                wait_ = false;
                current_set_time_ += s_to_time(86400); //one day
                return GPS_INTERVAL_STATUS::TO_NON_WAIT;
            }
            return GPS_INTERVAL_STATUS::WAIT;
        }
        //if one overflow -> current_time is smaller next time
        return GPS_INTERVAL_STATUS::WAIT;
    }
    //if overflow only current_time
    if (current_time < current_set_time_
        || next_time <= current_time)
    {
        wait_ = false;
        current_set_time_ += s_to_time(86400);
        return GPS_INTERVAL_STATUS::TO_NON_WAIT;
    }
    return GPS_INTERVAL_STATUS::WAIT;
}