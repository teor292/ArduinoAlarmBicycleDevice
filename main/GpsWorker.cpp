#include "GpsWorker.h"

#if defined(GPS)

GPSSettings::GpsSettings()
{
    //TODO load from EEPROM
}

void GPSSettings::Save()
{
    //TODO save to EEPROM
}

GpsWorker::GpsWorker(Stream& stream)
    : stream_(stream)
{

}

void GpsWorker::ReadFromStreamIfAvailable()
{
    while (stream.available())
    {
        gps_.encode((char)stream.read());
    }
}

void GpsWorker::Work()
{
    ReadFromStreamIfAvailable();
    //TODO
}

void GpsWorker::GetLocationString(const char* phone, SafeString& buffer)
{
    
}

#endif