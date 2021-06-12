#pragma once

enum class GPS_COMMANDS
{
    GET_GPS, //sms: get gps
    SET_GPS_REGIME, //sms like: set gps regime wait
    GET_GPS_REGIME, //sms like: get gps regime
    SET_GPS_REGIME_WAIT, //sms: set gps wait <mode> [<mode_on_alarm>]
    //available modes: all
    GET_GPS_REGIME_WAIT, //sms: get gps wait

    SET_GPS_REGIME_TRACK, //sms: set gps track <mode> [<mode_on_alarm>]
    //available modes: CONTINOUS, PSMCT
    GET_GPS_REGIME_TRACK, //sms: get gps track

    SET_GPS_REGIME_TRAIL, //sms: set gps trail <mode> [<mode_on_alarm>]
    //available modes: CONTINOUS, PSMCT, PSMOO
    GET_GPS_REGIME_TRAIL //sms: get gps trail
    

};