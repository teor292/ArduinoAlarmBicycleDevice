#pragma once

#include "gps_header.h"

#if defined(GPS)


#include "TinyGPS++.h"
#include <Stream.h>
#include "header.h"

class GpsWorker
{
    public:

        explicit GpsWorker(Stream& gps_stream);

        void Read();

        void Read(uint8_t c);

        void Work();

        void PerformCommand();


    protected:

        Stream& gps_stream_;
        TinyGPSPlus gps_;

};


#endif