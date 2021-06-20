#include "GPSSmsSender.h"

#if defined(GPS)

#include "GPSTextCreator.h"

GPSSmsSender::GPSSmsSender(Sms& sms)
    : sms_(sms)
{}

void GPSSmsSender::SetDefaultData(const SenderData& data)
{
    phone_ = data.values.phone;
}

void GPSSmsSender::SendGPS(TinyGPSPlus& gps)
{
    send_gps_(sms_, gps, phone_, 0);
}

void GPSSmsSender::SendGPS(Sms& sms, TinyGPSPlus& gps, const SenderData& data, uint32_t valid_age)
{
    send_gps_(sms, gps, data.values.phone, valid_age);
}

void GPSSmsSender::send_gps_(Sms& sms, TinyGPSPlus& gps, const Phone& phone, uint32_t valid_age)
{
    createSafeString(tmp, 255);
    GPSTextCreator::CreateGpsText(tmp, gps, valid_age);
    sms.SetPhone(phone.phone);
    sms.SendSms(tmp.c_str());
}

#endif