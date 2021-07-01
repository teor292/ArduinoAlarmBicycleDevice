#include "GPSTimeWrapperSender.h"

#if defined(GPS)

#include "time_utils.h"

GPSTimeWrapperSender::GPSTimeWrapperSender(AbstractGPSSender* sender, TinyGPSPlus& gps)
    : sender_(sender),
    gps_(gps)
{
}

void GPSTimeWrapperSender::SetSettings(const SendSettingData& settings)
{
    last_send_time_ = 0;
    settings_ = settings;
    is_last_check_non_valid_ = false;
}

void GPSTimeWrapperSender::Work()
{
    //auto current_time = millis();
    auto current_time = time();
    if (0 != last_send_time_ && current_time - last_send_time_ < s_to_time(settings_.send_time)) return;

    if (gps_.location.age() > settings_.GetValidTimeMs())
    {
        is_last_check_non_valid_ = true;
        return;
    }

    sender_->SendGPS(gps_);

    last_send_time_ = current_time;
    is_last_check_non_valid_ = false;
}

uint32_t GPSTimeWrapperSender::GetNextDiffTime(uint32_t current_time)
{
    if (is_last_check_non_valid_) return ULONG_MAX;
    return get_next_diff_time(last_send_time_, s_to_time(settings_.send_time), current_time);
}


#endif