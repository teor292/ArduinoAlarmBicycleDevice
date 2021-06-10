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
};


//define work regime of gps (not module!)
enum class GPS_REGIME
{
    WAIT, //device is wait. GPS module can be in all GPS_DEVICE_WORK_MODE state
    TRACK, //CONTINOUS or PSMCT mode with writing gps data on low time interval (5 - 50 s)
    TRAIL, //CONTINOUS/PSMCT/PSMOO modes, writing gps data on big time interval (1 - 1440 (60 * 24) minutes)
};

struct GPSModeSettings
{
     //seconds for CONTINOUS/PSMCT, minutes for PSMOO
     //this is time for get gps data by current software
    int time_update{1};
    //gps fix by module in seconds
    int time_fix{1}; 
};

struct GPSAllModeSettings
{
    GPSModeSettings continous_mode_settings;
    GPSModeSettings psmct_mode_settings;
    GPSModeSettings psmoo_mode_settings;
};

struct GPSRegimeSettings
{
    GPS_DEVICE_WORK_MODE mode{GPS_DEVICE_WORK_MODE::CONTINOUS};

    //gps mode if vibro alarm is present
    //can't be worse by performance than current
    GPS_DEVICE_WORK_MODE mode_on_alarm{GPS_DEVICE_WORK_MODE::INVALID}; 
};

struct GPSAllRegimesSettings
{
    GPSRegimeSettings wait_settings;
    GPSRegimeSettings track_settings;
    GPSRegimeSettings trail_settings;
};

struct GPSSettings
{
    GPSSettings();
    void Save();
    GPS_REGIME current_regime{GPS_REGIME::WAIT};
    GPSAllModeSettings regimes_settings;
    GPSAllModeSettings modes_settings;
};

#endif