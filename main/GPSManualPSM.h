#pragma once

#include "gps_header.h"

#if defined(GPS)

#include "GPSAutoStater.h"

using GPSFixCallback = bool (*)(uint32_t valid_period_time);

class GPSManualPSM
{
    public:
        explicit GPSManualPSM(GPSAutoStater& stater, GPSFixCallback callback);

        void UpdateSettings(const GPSFixSettings& fix_settings);

        void Work();

    protected:
        //60 seconds valid gps fix time
        const uint32_t VALID_PERIOD_TIME = 60000;
        GPSAutoStater& stater_;
        GPSFixCallback callback_;
        GPSFixSettings fix_settings_;
        uint32_t last_time_{0};
        uint32_t next_diff_force_time_{0};
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