#include "GpsWorker.h"

#if defined(GPS)

GpsWorker::GpsWorker(Stream& stream)
    : gps_stream_(stream)
{

}

void GpsWorker::Read()
{
    while (gps_stream_.available())
    {
        gps_.encode((char)gps_stream_.read());
    }
}


#endif