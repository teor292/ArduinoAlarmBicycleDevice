#pragma once

#include "gps_define.h"

#if defined(GPS)

#include "gps_header.h"

class AbstractGPSTimeSendManager
{
    public:
       virtual ~AbstractGPSTimeSendManager(){}
       virtual void AddOrUpdateSender(const SendSettings& settings) = 0;
       virtual bool RemoveSender(const SendSettings& settings) = 0;
       virtual void Reset() = 0;
       virtual void Work() = 0;
};

#endif