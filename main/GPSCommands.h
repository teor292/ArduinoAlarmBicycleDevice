#pragma once

#include "gps_header.h"

#if defined(GPS)

#include "Phone.h"

enum class GPS_COMMANDS
{
    GET_GPS, //sms: get gps
    //gps fix must be valid in 1 minutes
    GET_GPS_RESET, //sms: get gps reset
    //reset previous GET_GPS command
    GET_LAST_GPS, //sms: get gps last
    //get last gps fix without update
    SET_GPS_FIX, //sms like: set gps fix 0-86400/1m-1440m/1h-24h
    GET_GPS_FIX, //sms: get gps fix
    SET_GPS_VIBRO, //sms like: set gps vibro on/max/off [1-10] 
    GET_GPS_VIBRO, //sms: get gps vibro
    SET_GPS_SMS_SEND, //sms like: set gps send sms me/phone 30m-1440m/1h-24h [valid_time 5-3600]
    GET_GPS_SMS_SEND, //sms like: get gps send sms
    SET_GPS_SMS_ALARM, //sms like: set gps send sms alarm me/phone on/off
    GET_GPS_SMS_ALARM, //sms like: get gps send sms alarm
    GPS_RESET_SETTINGS, //sms: gps reset settings
    GPS_RESET_DEVICE, //sms: gps reset device
    INVALID //invalid command

};

struct GPSCommandData
{
    GPS_COMMANDS cmd;
    union
    {
        uint32_t update_time; //seconds, for fix
        uint32_t alarm_time; //seconds, for vibro
    };
    union
    {
        Phone phone;
    };
};  

#endif