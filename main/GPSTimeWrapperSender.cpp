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
    waiter_.SetSettings(settings);
}

void GPSTimeWrapperSender::Work()
{
    //auto current_time = millis();
    auto current_time = time();
    if (0 != last_send_time_ && current_time - last_send_time_ < s_to_time(settings_.send_time)) return;

    auto waiter_result = waiter_.Update(current_time);

    if (gps_.location.age() > settings_.GetValidTimeMs())
    {
        is_last_check_non_valid_ = true;
        return;
    }

    last_send_time_ = current_time;
    is_last_check_non_valid_ = false;



    if (GPS_INTERVAL_STATUS::WAIT == waiter_result
        || GPS_INTERVAL_STATUS::TO_WAIT == waiter_result)
    {
        return;
    }

    sender_->SendGPS(gps_);
}

uint32_t GPSTimeWrapperSender::GetNextDiffTime(uint32_t current_time)
{
    //do not include waiter interval because it would be non simple algorithm
    if (is_last_check_non_valid_) return ULONG_MAX;
    return get_next_diff_time(last_send_time_, s_to_time(settings_.send_time), current_time);
}


#endif