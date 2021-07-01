#pragma once

#include "gps_header.h"

#if defined(GPS)

#include "GPSAbstractSender.h"
#include "TinyGPS_time.h"

//this class helps send gps data
//the sending period is counted from the last sending
class GPSTimeWrapperSender
{
    public:
        explicit GPSTimeWrapperSender(AbstractGPSSender* sender, TinyGPSPlus& gps);

        void SetSettings(const SendSettingData& settings);

        void Work();

        uint32_t GetNextDiffTime(uint32_t current_time);

    protected:

        AbstractGPSSender* sender_;
        TinyGPSPlus& gps_;
        SendSettingData settings_;
        uint32_t last_send_time_{0};
        //This variable is needed to determine 
        //when the microcontroller should exit the standby mode 
        //when manual PSM of the gps device is active.
        //if false -> time of this sender must considered
        //if true -> last gps data is invalid, mode is PSM -> do not cosidered
        bool is_last_check_non_valid_{false};

    
};

#endif