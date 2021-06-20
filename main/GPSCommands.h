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
    SET_GPS_FIX, //sms like: set gps fix 0-86400/0m-1440m/0h-24h
    GET_GPS_FIX, //sms: get gps fix
    SET_GPS_VIBRO, //sms like: set gps vibro on/max/off [60-600/1m-10m] 
    GET_GPS_VIBRO, //sms: get gps vibro
    SET_GPS_SMS_SEND, //sms like: set gps send sms me/phone 1800-86400/30m-1440m/1h-24h [age 5-3600/1m-60m/1h]
    GET_GPS_SMS_SEND, //sms like: get gps send sms
    // SET_GPS_SMS_ALARM, //sms like: set gps send sms alarm me/phone on/off
    // GET_GPS_SMS_ALARM, //sms like: get gps send sms alarm
    GPS_RESET_SETTINGS, //sms: gps reset settings
    GPS_RESET_DEVICE, //sms: gps reset device
    INVALID //invalid command

};

struct GPSCommandData
{
    GPS_COMMANDS cmd;
    union
    {
        uint32_t update_time{0}; //seconds, for fix
        uint32_t alarm_time; //seconds, for vibro
    };
    //phone that is set by command or source if not support or not set by command
    Phone phone;
    //phone that is set by command
    Phone dst_phone;
    union
    {
        uint32_t age_time{0};
        GPS_ALARM_MODE alarm_mode;
    };

    explicit GPSCommandData(GPS_COMMANDS command)
        : cmd(command)
    {}
    explicit GPSCommandData(GPS_COMMANDS command, uint32_t time)
        : cmd(command),
        update_time(time)
    {}
    explicit GPSCommandData(GPS_COMMANDS command, uint32_t time, const Phone& phone)
        : cmd(command),
        update_time(time),
        phone(phone)
    {}
    explicit GPSCommandData(GPS_COMMANDS command, uint32_t time, const Phone& phone, GPS_ALARM_MODE mode)
        : cmd(command),
        update_time(time),
        phone(phone),
        alarm_mode(mode)
    {}

    explicit GPSCommandData(GPS_COMMANDS command, uint32_t time, const Phone& phone, const Phone& dst_phone, uint32_t age = 0)
        : cmd(command),
        update_time(time),
        phone(phone),
        dst_phone(dst_phone),
        age_time(age)

    {}

    GPSCommandData& operator = (const GPSCommandData& data)
    {
        cmd = data.cmd;
        update_time = data.update_time;
        phone = data.phone;
        age_time = data.age_time;
        return (*this);
    }
};  

#endif