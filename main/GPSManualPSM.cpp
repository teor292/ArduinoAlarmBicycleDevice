#include "GPSManualPSM.h"

#if defined(GPS)

#include <Arduino.h>
#include "header.h"

#define VALID_FIX_TIME 600000UL
#define TIME_ACK 600000UL

GPSManualPSM::GPSManualPSM(GPSAutoStater& stater, AbstractFixCallable* check)
    : stater_(stater),
    check_(check)
{}

void GPSManualPSM::UpdateSettings(const GPSFixSettings& fix_settings)
{
    if (force_activated_)
    {
        reset_force_();
    }
    fix_settings_ = fix_settings;
    last_time_ = millis() - fix_settings_.UpdateTimeMS();
    sum_diff_force_time_ = 0;
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
        PRINTLN("BY TIME");
        sum_diff_force_time_ = 0;
        last_time_ += fix_settings_.UpdateTimeMS();
        active_force_();
        return;
    }
    if (0 != sum_diff_force_time_ && time - (force_activate_time_ + TIME_ACK) > get_time_sleep_on_fail_())
    {
        PRINTLN("BY FIX");
        active_force_();
    }
}


void GPSManualPSM::check_for_fix_()
{
    if (check_->IsValidGPS(VALID_FIX_TIME + sum_diff_force_time_))
    {
        PRINTLN("GPS VALID");
        reset_force_();
        sum_diff_force_time_ = 0;
        return;
    }
    auto time = millis();
    if (time - force_activate_time_ > TIME_ACK)
    {
        PRINTLN("TIME ACK EXPIRED");
        reset_force_();
        sum_diff_force_time_ += get_time_sleep_on_fail_();
    }
}

uint32_t GPSManualPSM::get_time_sleep_on_fail_()
{
    if (2 * 60 * 60 <= fix_settings_.update_time) return 120UL * 60UL * 1000UL;
    if (1 * 60 * 60  <= fix_settings_.update_time) return 60UL * 60UL * 1000UL;
    return 30UL * 60UL * 1000UL;
}

void GPSManualPSM::reset_force_()
{
    stater_.ResetForce(GPS_STATER_FORCE::BY_SERVICE);
    force_activated_ = false;
}

void GPSManualPSM::active_force_()
{
    //if there is valid point by period of time it is not neccessary activate module
    if (check_->IsValidGPS(VALID_FIX_TIME + sum_diff_force_time_))
    {
        sum_diff_force_time_ = 0;
        return;
    }
    stater_.Force(GPS_STATER_FORCE::BY_SERVICE);
    force_activated_ = true;
    force_activate_time_ = millis();
}

bool GPSManualPSM::is_active_() const
{
    return 3600UL <= fix_settings_.update_time;
}

#endif