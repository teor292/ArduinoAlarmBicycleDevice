#pragma once

#include "gps_header.h"

#if defined(GPS)

#include "GPSAbstractSender.h"
#include <TinyGPS++.h>

class GPSTimeWrapperSender
{
    public:
        explicit GPSTimeWrapperSender(AbstractGPSSender* sender, TinyGPSPlus& gps);

        void SetSettings(const SendSettingData& settings);

        void Work();


    protected:

        AbstractGPSSender* sender_;
        TinyGPSPlus& gps_;
        SendSettingData settings_;
        uint32_t last_send_time_{0};

    
};

#endif