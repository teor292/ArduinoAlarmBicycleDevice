#include "GPSWorker.h"

#if defined(GPS)

GPSWorker::GPSWorker(Stream& stream, Sms& sms)
    : gps_stream_(stream),
    sms_(sms)
{

}

void GPSWorker::Read()
{
    while (gps_stream_.available())
    {
        gps_.encode((char)gps_stream_.read());
    }
}

void GPSWorker::Work(bool was_alarm)
{
    auto_stater_.Work(was_alarm);
    manual_psm_.Work();
}

void GPSWorker::PerformCommand(const GPSCommandData& command)
{
    
}

void GPSWorker::NonUBXSymbol(uint8_t c)
{
    gps_.encode(c);
}

bool GPSWorker::IsValidGPS(uint32_t valid_period_time)
{
    return gps_.location.age() < valid_period_time;
}

bool GPSWorker::CheckAge(uint32_t valid_time)
{
    return IsValidGPS(valid_time);
}

void GPSWorker::Send(const Phone& phone, bool valid)
{
    
}

#endif