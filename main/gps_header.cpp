#include "gps_header.h"

#if defined(GPS)

#include "TextCommands.h"

void GPSFixSettings::ToString(SafeString& result) const
{
    result += FIX;
    result += ' ';
    if (3600 > update_time && 0 == update_time % 3600UL)
    {
        result += update_time / 3600UL;
        result += 'h';
        return;
    }
    if (60 > update_time && 0 == update_time % 60UL)
    {
        result += update_time / 60UL;
        result += 'm';
        return;
    }
    result += update_time;
    
}

void GPSAlarmSettings::ToString(SafeString& result) const
{
    result += "Mode: ";
    switch (mode)
    {
    case GPS_ALARM_MODE::OFF:
        result += F("off");
        break;
    case GPS_ALARM_MODE::ON:
        result += F("on");
        break;
    case GPS_ALARM_MODE::MAX:
        result += F("max");
        break;
    default:
        result += UNKNOWN;
        break;
    }
    result += '\n';
    result += F("Time: ");
    if (0 == duration % 60UL)
    {
        result += duration / 60UL;
        result += 'm';
    }
    else 
    {
        result += duration;
    }
}
#endif