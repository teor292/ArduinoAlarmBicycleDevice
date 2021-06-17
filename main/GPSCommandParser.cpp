#include "GPSCommandParser.h"

#if defined(GPS)

#include "TextCommands.h"

const char GET[] = "get";
const char SET[] = "set";
const char GPS1[] = "gps";
const char LAST[] = "last";
const char FIX[] = "fix";
const char RESET[] = "reset";
const char VIBRO[] = "vibro";
const char MAX[] = "max";
const char SMS[] = "sms";
const char ME[] = "me";
const char PHONE[] = "phone";
const char SEND[] = "send";
const char ALARM[] = "alarm";

GPSCommandData GPSCommandParser::ParseSms(SafeString& sms_string, Phone& source_phone)
{
    sms_string.trim();
    if (sms_string.startsWith(GET))
    {
        return parse_get_(sms_string, source_phone);
    }
    if (sms_string.startsWith(SET))
    {
        return parse_set_(sms_string, source_phone);
    }
    if (sms_string.startsWith(GPS1))
    {
        return parse_gps_(sms_string, source_phone);
    }
    return GPSCommandData{GPS_COMMANDS::INVALID};
}

GPSCommandData GPSCommandParser::parse_get_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(GET));
    if (!sms_string.startsWith(GPS1)) return GPSCommandData{GPS_COMMANDS::INVALID};
    if (sms_string.length() == sizeof(GPS1) - 1) return GPSCommandData{GPS_COMMANDS::GET_GPS, 0, source_phone};
    sms_string.substring(sms_string, sizeof(GPS1));
    if (sms_string == LAST) return GPSCommandData{GPS_COMMANDS::GET_LAST_GPS, 0, source_phone};
    if (sms_string == RESET) return GPSCommandData{GPS_COMMANDS::GET_GPS_RESET, 0, source_phone};
    if (sms_string == FIX) return GPSCommandData{GPS_COMMANDS::GET_GPS_FIX, 0, source_phone};
    if (sms_string == VIBRO) return GPSCommandData{GPS_COMMANDS::GET_GPS_VIBRO, 0, source_phone};
    if (sms_string.startsWith(SEND)) 
    {
        return parse_get_send_(sms_string, source_phone);
    }

    return GPSCommandData{GPS_COMMANDS::INVALID};
   
}

GPSCommandData GPSCommandParser::parse_set_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(SET));
    if (!sms_string.startsWith(GPS1)) return GPSCommandData{GPS_COMMANDS::INVALID};
    sms_string.substring(sms_string, sizeof(GPS1));


    /*
    SET_GPS_FIX, //sms like: set gps fix 0-86400/1m-1440m/1h-24h
    SET_GPS_VIBRO, //sms like: set gps vibro on/max/off [1-10] 
    SET_GPS_SMS_SEND, //sms like: set gps send sms me/phone 30m-1440m/1h-24h [time 5-3600]
    SET_GPS_SMS_ALARM, //sms like: set gps send sms alarm me/phone on/off
    GPS_RESET_SETTINGS, //sms: gps reset settings
    GPS_RESET_DEVICE, //sms: gps reset device
    INVALID //invalid command
    */

}

GPSCommandData GPSCommandParser::parse_get_send_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(SEND));
    if (sms_string == SMS) return GPSCommandData{GPS_COMMANDS::GET_GPS_SMS_SEND, 0, source_phone};
    if (!sms_string.startsWith(SMS))
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }

    sms_string.substring(sms_string, sizeof(SMS));
    if (sms_string == ALARM) return GPSCommandData{GPS_COMMANDS::GET_GPS_SMS_ALARM, 0, source_phone};

    return GPSCommandData{GPS_COMMANDS::INVALID};
}

#endif