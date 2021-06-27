#include "GPSCommandParser.h"

#if defined(GPS)



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
const char AGE[] = "age";
const char SETTINGS[] = "settings";
const char DEVICE[] = "device";
const char REMOVE[] = "remove";

namespace
{
    const char ON[] = "on";
    const char OFF[] = "off";
    const char GET[] = "get";
    const char SET[] = "set";
}




GPSCommandData GPSCommandParser::ParseSms(SafeString& sms_string, Phone& source_phone)
{
    sms_string.trim();
    sms_string.toLowerCase();
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

GPSCommandData GPSCommandParser::parse_gps_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(GPS1));

    if (!sms_string.startsWith(RESET))
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }
    sms_string.substring(sms_string, sizeof(RESET));
    if (sms_string == SETTINGS)
    {
        return GPSCommandData{GPS_COMMANDS::GPS_RESET_SETTINGS, 0, source_phone};
    }
    if (sms_string == DEVICE)
    {
        return GPSCommandData{GPS_COMMANDS::GPS_RESET_DEVICE, 0, source_phone};
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
    if (sms_string.startsWith(FIX))
    {
        return parse_set_fix_(sms_string, source_phone);
    }
    if (sms_string.startsWith(VIBRO))
    {
        return parse_set_vibro_(sms_string, source_phone);
    }
    if (sms_string.startsWith(SEND))
    {
        return parse_set_send_(sms_string, source_phone);
    }

}

// GPSCommandData GPSCommandParser::parse_set_send_alarm_(SafeString& sms_string, Phone& source_phone)
// {
//     sms_string.substring(sms_string, sizeof(ALARM));
//     Phone phone = source_phone;
//     if (!sms_string.startsWith(ME))
//     {
//         if (!sms_string.startsWith("+"))
//         {
//             return GPSCommandData{GPS_COMMANDS::INVALID};
//         }
//         int index = sms_string.indexOf(' ');
//         if (-1 == index)
//         {
//             return GPSCommandData{GPS_COMMANDS::INVALID};
//         }
//         if (!phone.AssignData(sms_string.c_str(), index))
//         {
//             return GPSCommandData{GPS_COMMANDS::INVALID};
//         }
//         sms_string.substring(sms_string, index + 1);
//     } 
//     else
//     {
//         sms_string.substring(sms_string, sizeof(ME));
//     }
//     if (sms_string == ON)
//     {
//         return GPSCommandData{GPS_COMMANDS::SET_GPS_SMS_ALARM, 0, phone, GPS_ALARM_MODE::ON};
//     }
//     if (sms_string == OFF)
//     {
//         return GPSCommandData{GPS_COMMANDS::SET_GPS_SMS_SEND, 0, phone, GPS_ALARM_MODE::OFF};
//     }
//     return GPSCommandData{GPS_COMMANDS::INVALID};
// }

GPSCommandData GPSCommandParser::parse_set_send_remove_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(REMOVE));
    Phone dst_phone = source_phone;
    if (!sms_string.startsWith(ME))
    {
        if (!sms_string.startsWith("+"))
        {
            return GPSCommandData{GPS_COMMANDS::INVALID};
        }
        if (!dst_phone.AssignData(sms_string.c_str(), sms_string.length()))
        {
            return GPSCommandData{GPS_COMMANDS::INVALID};
        }
    } 
    return GPSCommandData{GPS_COMMANDS::SET_GPS_REMOVE_SMS_SEND, 0, source_phone, dst_phone};
}


GPSCommandData GPSCommandParser::parse_set_send_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(SEND));
    Phone dst_phone = source_phone;
    if (!sms_string.startsWith(SMS))
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }
    sms_string.substring(sms_string, sizeof(SMS));
    // if (sms_string.startsWith(ALARM))
    // {
    //     return parse_set_send_alarm_(sms_string, source_phone);
    // }
    if (sms_string.startsWith(REMOVE))
    {
        return parse_set_send_remove_(sms_string, source_phone);
    }
    if (!sms_string.startsWith(ME))
    {
        if (!sms_string.startsWith("+"))
        {
            return GPSCommandData{GPS_COMMANDS::INVALID};
        }
        int index = sms_string.indexOf(' ');
        if (-1 == index)
        {
            return GPSCommandData{GPS_COMMANDS::INVALID};
        }
        if (!dst_phone.AssignData(sms_string.c_str(), index))
        {
            return GPSCommandData{GPS_COMMANDS::INVALID};
        }
        sms_string.substring(sms_string, index + 1);
    } 
    else
    {
        sms_string.substring(sms_string, sizeof(ME));
    }
    createSafeString(time_str, 6);

    int tmp_index = sms_string.indexOf(' ');
    if (-1 == tmp_index)
    {
        time_str = sms_string;
    } else
    {
        sms_string.substring(time_str, 0, tmp_index);
    }

    uint32_t time = 0;
    if (!parse_time_(time_str, time))
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }
    if (300 > time || 86400 < time)
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }
    if (-1 == tmp_index)
    {
        return GPSCommandData{GPS_COMMANDS::SET_GPS_SMS_SEND, time, source_phone, dst_phone};
    }
    sms_string.substring(sms_string, tmp_index + 1);
    if (!sms_string.startsWith(AGE))
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }
    sms_string.substring(sms_string, sizeof(AGE));
    uint32_t age_time = 0;
    if (!parse_time_(sms_string, age_time))
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }
    if (5 > age_time || 3600 < age_time)
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }
    return GPSCommandData{GPS_COMMANDS::SET_GPS_SMS_SEND, time, source_phone, dst_phone, age_time};

}

GPSCommandData GPSCommandParser::parse_set_vibro_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(VIBRO));
    GPS_ALARM_MODE alarm_mode;
    size_t sz;
    if (sms_string.startsWith(ON))
    {
        alarm_mode = GPS_ALARM_MODE::ON;
        sz = sizeof(ON);
    } 
    else if (sms_string.startsWith(MAX))
    {
        alarm_mode = GPS_ALARM_MODE::MAX;
        sz = sizeof(MAX);
    }
    else if (sms_string == OFF)
    {
        alarm_mode = GPS_ALARM_MODE::OFF;
        sz = sizeof(OFF);
    }
    else
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }
    if (sms_string.length() == sz - 1)
    {
        return GPSCommandData{GPS_COMMANDS::SET_GPS_VIBRO, 0, source_phone, alarm_mode};
    }
    sms_string.substring(sms_string, sz);
    uint32_t time = 0;
    if (!parse_time_(sms_string, time))
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }
    if (60 > time || 600 < time)
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }
    return GPSCommandData{GPS_COMMANDS::SET_GPS_VIBRO, time, source_phone, alarm_mode};
}

bool GPSCommandParser::parse_time_(SafeString& sms_string, uint32_t& result)
{
    //negative values not supported
    if ('-' == sms_string[0]) return false;

    auto length = sms_string.length();
    auto c = sms_string[length - 1];
    uint32_t koef = 1UL;
    if ('m' == c || 'h' == c)
    {
        sms_string.substring(sms_string, 0, length - 1);
        koef = 'm' == c ? 60UL : 3600UL;
    } 
    long tmp = 0;
    if (!sms_string.toLong(tmp)) return false;

    //case to seconds
    result = static_cast<uint32_t>(tmp) * koef;
    return true;
}

GPSCommandData GPSCommandParser::parse_set_fix_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(FIX));
    uint32_t time = 0;
    if (!parse_time_(sms_string, time)) return GPSCommandData{GPS_COMMANDS::INVALID};

    if (86400 < time) return  GPSCommandData{GPS_COMMANDS::INVALID};

    return GPSCommandData{GPS_COMMANDS::SET_GPS_FIX, time, source_phone};
}

GPSCommandData GPSCommandParser::parse_get_send_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(SEND));
    if (sms_string == SMS) return GPSCommandData{GPS_COMMANDS::GET_GPS_SMS_SEND, 0, source_phone};
    if (!sms_string.startsWith(SMS))
    {
        return GPSCommandData{GPS_COMMANDS::INVALID};
    }

    // sms_string.substring(sms_string, sizeof(SMS));
    // if (sms_string == ALARM) return GPSCommandData{GPS_COMMANDS::GET_GPS_SMS_ALARM, 0, source_phone};

    return GPSCommandData{GPS_COMMANDS::INVALID};
}

#endif