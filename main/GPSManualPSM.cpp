#include "GPSManualPSM.h"

#if defined(GPS)

#include <Arduino.h>
#include "header.h"
#include "time_utils.h"

#define VALID_FIX_TIME_MS 600000UL
#define TIME_ACK_S 600UL

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
    last_time_ = time() - s_to_time(fix_settings_.update_time);
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

uint32_t GPSManualPSM::NextDiffAwakeTime(uint32_t current_time)
{
    if (!is_active_()) return ULONG_MAX;
    if (force_activated_) return 0;

    auto next_time = last_time_ + s_to_time(fix_settings_.update_time);
    bool was_overflow = next_time < last_time_;
    if (0 != sum_diff_force_time_)
    {
        auto tmp = get_time_sleep_on_fail_() + force_activate_time_ + s_to_time(TIME_ACK_S);
        bool was_overflow2 = tmp < force_activate_time_;

        if (was_overflow && !was_overflow2)
        {
            //tmp is less
            next_time = tmp;
            was_overflow = was_overflow2; //false
        }
        else if (was_overflow == was_overflow2)
        {
            if (tmp < next_time)
            {
                next_time = tmp;
                was_overflow = was_overflow2;
            }
        }// in other cases -> do nothing
    }

    //current_time overflow
    if (current_time < last_time_)
    {
        if (was_overflow)
        {
            if (next_time < current_time) return 0;
            return next_time - current_time;
        }
        return 0;
    }
    //current_time is not overflow and next_time can be overflowed or can't be overflowed
    return next_time - current_time;
}

void GPSManualPSM::force_check_start_()
{
    //auto current_time = millis();
    auto current_time = time();
    if (current_time - last_time_ > s_to_time(fix_settings_.update_time))
    {
        PRINTLN("BY TIME");
        sum_diff_force_time_ = 0;
        last_time_ += s_to_time(fix_settings_.update_time);
        active_force_();
        return;
    }
    if (0 != sum_diff_force_time_ && current_time - (force_activate_time_ + s_to_time(TIME_ACK_S)) > get_time_sleep_on_fail_())
    {
        PRINTLN("BY FIX");
        active_force_();
    }
}


void GPSManualPSM::check_for_fix_()
{
    if (check_->IsValidGPS(VALID_FIX_TIME_MS + sum_diff_force_time_))
    {
        PRINTLN("GPS VALID");
        reset_force_();
        sum_diff_force_time_ = 0;
        return;
    }
    //auto current_time = millis();
    auto current_time = time();
    if (current_time - force_activate_time_ > TIME_ACK_S)
    {
        PRINTLN("TIME ACK EXPIRED");
        reset_force_();
        sum_diff_force_time_ += time_to_ms(get_time_sleep_on_fail_());
    }
}

uint32_t GPSManualPSM::get_time_sleep_on_fail_()
{
    if (2 * 60 * 60 <= fix_settings_.update_time) return s_to_time(120UL * 60UL);
    if (1 * 60 * 60  <= fix_settings_.update_time) return s_to_time(60UL * 60UL);
    return s_to_time(30UL * 60UL);
}

void GPSManualPSM::reset_force_()
{
    stater_.ResetForce(GPS_STATER_FORCE::BY_SERVICE);
    force_activated_ = false;
}

void GPSManualPSM::active_force_()
{
    //if there is valid point by period of time it is not neccessary activate module
    if (check_->IsValidGPS(VALID_FIX_TIME_MS + sum_diff_force_time_))
    {
        sum_diff_force_time_ = 0;
        return;
    }
    stater_.Force(GPS_STATER_FORCE::BY_SERVICE);
    force_activated_ = true;
    force_activate_time_ = time();
}

bool GPSManualPSM::is_active_() const
{
    return 3600UL <= fix_settings_.update_time;
}

#endif