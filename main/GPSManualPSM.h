#pragma once

#include "gps_header.h"

#if defined(GPS)

#include "GPSAutoStater.h"

class AbstractFixCallable
{
    public:
        
        virtual ~AbstractFixCallable(){}
        virtual bool IsValidGPS(uint32_t valid_period_time_ms) = 0;
};

//class for manual PSM mode
//awake gps device every N time
//The period of the next awakening depends on whether the coordinates were received. 
//However, regardless of this, the device will still wake up with a N time period.
class GPSManualPSM
{
    public:
        explicit GPSManualPSM(GPSAutoStater& stater, AbstractFixCallable* check);

        void UpdateSettings(const GPSFixSettings& fix_settings);

        void Work();

        uint32_t NextDiffAwakeTime(uint32_t current_time);

        bool IsActive() const
        {
            return is_active_();
        }

    protected:

        GPSAutoStater& stater_;
        AbstractFixCallable* check_;
        GPSFixSettings fix_settings_;
        uint32_t last_time_{0};
        uint32_t sum_diff_force_time_{0};
        uint32_t force_activate_time_{0};
        bool force_activated_{false};

        bool is_active_() const;

        void check_for_fix_();
        void force_check_start_();
        uint32_t get_time_sleep_on_fail_();
        void reset_force_();
        void active_force_();

};


#endif