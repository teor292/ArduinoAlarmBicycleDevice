#include "GPSSmsSender.h"

#if defined(GPS)

#include "GPSTextCreator.h"

GPSSmsSender::GPSSmsSender(Sms& sms)
    : sms_(sms)
{}

void GPSSmsSender::SetDefaultData(const SenderData& data)
{
    phone_ = data.phone;
}

void GPSSmsSender::SendGPS(TinyGPSPlus& gps)
{
    send_gps_(gps, phone_, 0);
}

void GPSSmsSender::SendGPS(TinyGPSPlus& gps, const SenderData& data, uint32_t valid_age)
{
    send_gps_(gps, data.phone, valid_age);
}

void GPSSmsSender::send_gps_(TinyGPSPlus& gps, const Phone& phone, uint32_t valid_age)
{
    createSafeString(tmp, 255);
    GPSTextCreator::CreateGpsText(tmp, gps, valid_age);
    sms_.SetPhone(phone.phone);
    sms_.SendSms(tmp.c_str());
}

#endif