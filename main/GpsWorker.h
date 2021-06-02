#pragma once

#if defined(GPS)

#include "gps_header.h"
#include "TinyGps++.h"
#include <Stream.h>
#include "header.h"

class GpsWorker
{
    public:

        explicit GpsWorker(Stream& stream);

        void ReadFromStreamIfAvailable();

        void Work();

        void GetLocationString(const char* phone, SafeString& buffer);

        TinyGPSPlus& Gps() const
        {
            return gps_;
        }

    protected:

        enum class CURRENT_STATE
        {
            NONE,
            WAIT_FOR_GPS
        };

        Stream& stream_;
        GPSSettings settings_;
        TinyGPSPlus gps_;
        CURRENT_STATE state_{CURRENT_STATE::NONE};
        char last_phone_[PHONE_BUF_LENGTH];

};


#endif