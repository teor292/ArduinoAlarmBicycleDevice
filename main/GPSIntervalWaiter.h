#pragma once

#include "gps_define.h"


#if defined(GPS)

#include "gps_header.h"

enum class GPS_INTERVAL_STATUS
{
    NON_WAIT, 
    TO_WAIT, //change from NON_WAIT to TO_WAIT
    WAIT,
    TO_NON_WAIT
};

class GPSIntervalWaiter
{
    public:

        void SetSettings(const SendSettingData& settings);

        GPS_INTERVAL_STATUS Update(uint32_t current_time);

        bool IsWait() const
        {
            return wait_;
        }
    
    private:
        uint32_t current_set_time_{0};
        uint32_t interval_delay_{0};
        uint32_t interval_wait_{0};
        bool wait_{false};

        GPS_INTERVAL_STATUS non_wait_update_(uint32_t current_time);
        GPS_INTERVAL_STATUS wait_update_(uint32_t current_time);

};

#endif