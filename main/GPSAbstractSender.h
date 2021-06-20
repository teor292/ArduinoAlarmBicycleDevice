#pragma once

#include "gps_header.h"

#if defined(GPS)

#include "TinyGPS++.h"
#include "Phone.h"

class AbstractGPSSender
{
    public:
        virtual ~AbstractGPSSender(){}
        virtual void SetDefaultData(const SenderData& data) = 0;
        virtual void SendGPS(TinyGPSPlus& gps) = 0;
};  

#endif