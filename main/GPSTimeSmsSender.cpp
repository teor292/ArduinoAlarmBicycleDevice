#include "GPSTimeSmsSender.h"

#if defined(GPS)


GPSTimeSmsSender::GPSTimeSmsSender(Sms& sms, TinyGPSPlus& gps)
    : sender_(sms),
        worker_(&sender_, gps)
{}

void GPSTimeSmsSender::SetSettings(const SendSettings& settings)
{
    sender_.SetDefaultData(settings.send_data);
    worker_.SetSettings(settings.Data());
}

void GPSTimeSmsSender::Work()
{
    worker_.Work();
}

#endif