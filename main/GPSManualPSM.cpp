#include "GPSManualPSM.h"

#if defined(GPS)

#include <Arduino.h>

#define VALID_FIX_TIME 600000UL

GPSManualPSM::GPSManualPSM(GPSAutoStater& stater, GPSFixCallback callback)
    : stater_(stater),
    callback_(callback)
{}

void GPSManualPSM::UpdateSettings(const GPSFixSettings& fix_settings)
{
    if (force_activated_)
    {
        reset_force_();
    }
    fix_settings_ = fix_settings;
    last_time_ = millis() - fix_settings_.UpdateTimeMS();
    next_diff_force_time_ = 0;
}

void GPSManualPSM::Work()
{
    if (!is_active_()) return;

    if (force_activated_)
    {
        check_for_fix_();
        return;
    }
    force_check_start_();
}

void GPSManualPSM::force_check_start_()
{
    auto time = millis();
    if (time - last_time_ > fix_settings_.UpdateTimeMS())
    {
        next_diff_force_time_ = 0;
        last_time_ += fix_settings_.UpdateTimeMS();
        active_force_();
        return;
    }
    if (0 != next_diff_force_time_ && time - last_time_ > next_diff_force_time_)
    {
        active_force_();
    }
}


void GPSManualPSM::check_for_fix_()
{
    if (callback_(VALID_FIX_TIME + next_diff_force_time_))
    {
        reset_force_();
        next_diff_force_time_ = 0;
        return;
    }
    auto time = millis();
    if (time - force_activate_time_ > 600000UL)
    {
        reset_force_();
        next_diff_force_time_ += get_time_sleep_on_fail_();
    }
}

uint32_t GPSManualPSM::get_time_sleep_on_fail_()
{
    if (240 <= fix_settings_.update_time) return 120UL * 60UL * 1000UL;
    if (120 <= fix_settings_.update_time) return 60UL * 60UL * 1000UL;
    return 30UL * 60UL * 1000UL;
}

void GPSManualPSM::reset_force_()
{
    stater_.ResetForce(GPS_STATER_FORCE::BY_SERVICE);
    force_activated_ = false;
}

void GPSManualPSM::active_force_()
{
    stater_.Force(GPS_STATER_FORCE::BY_SERVICE);
    force_activated_ = true;
    force_activate_time_ = millis();
}

bool GPSManualPSM::is_active_() const
{
    return 3600UL <= fix_settings_.update_time;
}

#endif