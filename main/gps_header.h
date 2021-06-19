#pragma once

//gps support only with SAMD (not enough memory in arduino for all functions)
#if defined(__SAMD21G18A__)
#define GPS
#endif


#if defined(GPS)

#include <stdint.h>
#include <SafeString.h>
#include "TextCommands.h"

//define work mode of gps module
enum class GPS_DEVICE_WORK_MODE
{
    CONTINOUS = 0, //default, no energy saving
    PSMCT, //short update period (< 10 seconds)
    PSMOO, //long update (> 10 second)
    OFF, //OFF by command || power,
    LAST = OFF //service variable
};

struct GPSFixSettings
{
    //fix gps coordinate interval, in seconds
    //0 - disabled (OFF mode)
    //1 - 9 CONTINOUS mode (rate 1 - 9)
    //10 - 59 - PSMCT (ack rate 5, update 5, doNotEnterOff flag enable)
    //60 - 299 - PSMOO (ack rate 5, doNotEnterOff flag enable)
    //300 - 599 - PSMOO (ack rate 5, searchPeriod 5 minutes)
    //600s - 59 min - PSMOO (ack rate 5, searchPeriod 10 minutes)
    //60 min - 24 h - manual (awake after time in CONTINOUS for 10 minutes,
    //if not found -> sleep: 
    // >=240 min = 2 h
    // >= 120 min = 1 h
    // >= 60 min = 30 min)
    uint32_t update_time{0}; 

    uint32_t UpdateTimeMS() const
    {
        return update_time * 1000UL;
    }

    void ToString(SafeString& result) const;
};

enum class GPS_ALARM_MODE
{
    OFF = 0, //disabled change state on alarm
    ON = 1, //enabled in PSCMT, update - 5 seconds, doNotEnterOff flag enable
    //if already in PSMCT with doNotEnterOff flag enabled || continous mode -> do nothing
    MAX = 2 //enabled in continous mode, rate 5
    //if already in continous with rate 1-5 -> do nothing
};

//settings how to work gps if was alarm
struct GPSAlarmSettings
{
    GPS_ALARM_MODE mode{GPS_ALARM_MODE::OFF};
    //duration of 'mode' after last alarm, default - 3 min
    uint8_t duration{180};

    uint32_t GetDurationMs() const
    {
        return 1000UL * duration;
    }

    void Check()
    {
        if (60 > duration || 600 < duration)
        {
            duration = 180;
        }
    }

    void ToString(SafeString& result) const;
};

//convience struct for settings
struct GPSStateSettings
{
    GPSFixSettings fix_settings;
    GPSAlarmSettings alarm_settings;
};


struct GPSSettings
{
    GPSSettings(){}
    void Save(){}
    GPSStateSettings state_settings;
};

#endif