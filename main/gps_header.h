#pragma once

//gps support only with SAMD (not enough memory in arduino for all functions)
#if defined(__SAMD21G18A__)
#define GPS
#endif


#if defined(GPS)

//define work mode of gps module
enum class GPS_DEVICE_WORK_MODE
{
    CONTINOUS, //default, no energy saving
    PSMCT, //short update period (< 10 seconds)
    PSMOO, //long update (> 10 second)
    SOFTWARE_OFF, //OFF by command,
    HARDWARE_OFF, //OFF by disable power
    INVALID = 0xFF //for alarm by vibro (see GPSRegimeSettings)
}

//define work regime of gps (not module!)
enum class GPS_REGIME
{
    WAIT, //device is wait. GPS module can be in all GPS_DEVICE_WORK_MODE state
    TRACK, //CONTINOUS or PSMCT mode with writing gps data on low time interval (5 - 50 s)
    TRAIL, //CONTINOUS/PSMCT/PSMOO modes, writing gps data on big time interval (1 - 1440 (60 * 24) minutes)
};

struct GPSRegimeSettings
{
    GPS_DEVICE_WORK_MODE mode;
    int time_update; //seconds for PSMCT, minutes for PSMOO

    //gps mode if vibro alarm is present
    //can't be worse by performance than current
    GPS_DEVICE_WORK_MODE mode_on_alarm; 
};

struct GPSSettings
{
    GPSSettings();
    void Save();
    GPS_REGIME current_regime{GPS_WORK_MODE::WAIT};
    GPSRegimeSettings wait_settings;
    GPSRegimeSettings track_settings;
    GPSRegimeSettings trail_settings;
};

#endif