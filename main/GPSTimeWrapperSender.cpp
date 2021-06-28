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
}

void GPSTimeWrapperSender::Work()
{
    //auto current_time = millis();
    auto current_time = time();
    if (0 != last_send_time_ && current_time - last_send_time_ < settings_.GetSendTimeMs()) return;

    if (gps_.location.age() > settings_.GetValidTimeMs()) return;

    sender_->SendGPS(gps_);

    last_send_time_ = current_time;
}


#endif