#pragma once

enum class GPS_COMMANDS
{
    GET_GPS, //sms: get gps
    SET_GPS_FIX, //sms like: set gps fix 0-8600/1m-1440m/1h-24h
    GET_GPS_FIX, //sms: get gps fix
    SET_GPS_VIBRO, //sms like: set gps vibro on/max/off [1-10] 
    GET_GPS_VIBRO, //sms: get gps vibro
   

};