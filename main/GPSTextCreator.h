#pragma once

#include "gps_header.h"

#if defined(GPS)

#include "TinyGPS_time.h"
#include <SafeString.h>

class GPSTextCreator
{
    public:

        static void CreateGpsText(SafeString& text, TinyGPSPlus& gps, uint32_t valid_age = 0);

    private:

        static void append_age_(SafeString& text, uint32_t time);
        static void add_value_(SafeString& text, uint32_t value);
};

#endif