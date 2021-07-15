#include "GPSIntervalWaiter.h"

#if defined(GPS)


#include "time_utils.h"
#include "header.h"

void GPSIntervalWaiter::SetSettings(const SendSettingData& settings)
{
    current_set_time_ = time();
    interval_delay_ = s_to_time(settings.interval_delay_time);
    interval_wait_ = s_to_time(settings.interval_not_send_time);
    wait_ = false;
    PRINT(F("CUR TIME: "));
    PRINTLN(current_set_time_);
    PRINT(F("DELAY: "));
    PRINTLN(interval_delay_);
    PRINT(F("WAIT: "));
    PRINTLN(interval_wait_);
}

GPS_INTERVAL_STATUS GPSIntervalWaiter::Update(uint32_t current_time)
{
    if (0 == interval_wait_) return GPS_INTERVAL_STATUS::NON_WAIT;
    if (wait_) return wait_update_(current_time);
    return non_wait_update_(current_time);
}

GPS_INTERVAL_STATUS GPSIntervalWaiter::non_wait_update_(uint32_t current_time)
{
    PRINTLN(F("NON WAIT"));
    PRINT(current_time);
    PRINT(F(" vs "));
    auto next_time = current_set_time_ + interval_delay_;
    PRINTLN(next_time);
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
                PRINTLN(F("TO WAIT 2 OVERFLOW"));
                return GPS_INTERVAL_STATUS::TO_WAIT;
            }
            PRINTLN(F("NON_WAIT 2 OVERFLOW"));
            return GPS_INTERVAL_STATUS::NON_WAIT;
        }
        PRINTLN(F("NON_WAIT 1 OVERFLOW"));
        //if one overflow -> current_time is smaller next time
        return GPS_INTERVAL_STATUS::NON_WAIT;
    }
    //if overflow only current_time
    if (current_time < current_set_time_
        || next_time <= current_time)
    {
        PRINTLN(F("TO_WAIT COMPARE"));
        wait_ = true;
        return GPS_INTERVAL_STATUS::TO_WAIT;
    }
    PRINTLN(F("NON_WAIT COMPARE"));
    return GPS_INTERVAL_STATUS::NON_WAIT;
}

GPS_INTERVAL_STATUS GPSIntervalWaiter::wait_update_(uint32_t current_time)
{
    PRINTLN(F("WAIT upd"));
    auto next_time = current_set_time_ + interval_delay_ + interval_wait_;
    PRINT(current_time);
    PRINT(F(" vs "));
    PRINTLN(next_time);
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
                PRINTLN(F("TO NON WAIT 2 OVERFLOW"));
                return GPS_INTERVAL_STATUS::TO_NON_WAIT;
            }
            PRINTLN(F("WAIT 2 OVERFLOW"));
            return GPS_INTERVAL_STATUS::WAIT;
        }
        PRINTLN(F("WAIT 1 OVERFLOW"));
        //if one overflow -> current_time is smaller next time
        return GPS_INTERVAL_STATUS::WAIT;
    }
    //if overflow only current_time
    if (current_time < current_set_time_
        || next_time <= current_time)
    {
        PRINTLN(F("TO NON WAIT COMPARE"));
        wait_ = false;
        current_set_time_ += s_to_time(86400);
        return GPS_INTERVAL_STATUS::TO_NON_WAIT;
    }
    PRINTLN(F("WAIT COMPARE"));
    return GPS_INTERVAL_STATUS::WAIT;
}

#endif