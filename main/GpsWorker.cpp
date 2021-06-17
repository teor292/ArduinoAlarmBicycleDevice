#include "GPSWorker.h"

#if defined(GPS)

GPSWorker::GPSWorker(Stream& stream, Sms& sms, WaitCallback wait_callback)
    : gps_stream_(stream),
    auto_stater_(stream, this, wait_callback),
    manual_psm_(auto_stater_, this),
    sms_sender_(sms)
{
    senders_.push_back(&sms_sender_);
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