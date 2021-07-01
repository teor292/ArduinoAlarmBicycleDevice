#pragma once

#include "gps_define.h"

#if defined(GPS)

#include "GPSSmsSender.h"
#include "GPSTimeWrapperSender.h"
#include "TinyGPS_time.h"

class GPSTimeSmsSender
{
    public:

        explicit GPSTimeSmsSender(Sms& sms, TinyGPSPlus& gps);

        void SetSettings(const SendSettings& settings);

        const Phone& GetPhone() const
        {
            return sender_.GetPhone();
        }

        void Work();

        bool operator == (const Phone& phone) const
        {
            return sender_ == phone;
        }

        uint32_t GetNextDiffTime(uint32_t current_time)
        {
            return worker_.GetNextDiffTime(current_time);
        }
    
    protected:

        GPSSmsSender sender_;
        GPSTimeWrapperSender worker_;
};


#endif