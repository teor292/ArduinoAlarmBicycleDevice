#include "DefaultCommandParser.h"


namespace
{
    const char ON[] = "on";
    const char OFF[] = "off";
    const char GET[] = "get";
    const char SET[] = "set";
    const char BATTERY[] = "battery";
    const char TIME[] = "time";
    const char SIGNAL[] = "signal";
    const char ADMIN[] = "admin";
    const char LOW1[] = "low";
    const char ALARM[] = "alarm";
    const char MODE[] = "mode";
    const char DEF[] = "def";
    const char SLEEP[] = "sleep";
    const char SENSITY[] = "sensity";
}




DefaultCommandData DefaultCommandParser::ParseSms(SafeString& sms_string, Phone& source_phone)
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
    return DefaultCommandData{source_phone};
}


DefaultCommandData DefaultCommandParser::parse_set_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(SET));
    if (sms_string == ADMIN)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::SET_ADMIN, source_phone};
    }
    if (sms_string.startsWith(LOW1))
    {
        return parse_set_low_(sms_string, source_phone);
    }
    if (sms_string.startsWith(ALARM))
    {
        return parse_set_alarm_(sms_string, source_phone);
    }
    if (sms_string.startsWith(MODE))
    {
        return parse_set_mode_(sms_string, source_phone);
    }

    return DefaultCommandData{source_phone};
}

DefaultCommandData DefaultCommandParser::parse_set_mode_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(MODE));
    if (sms_string == DEF)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::SET_MODE, source_phone, WORK_MODE::STANDART};
    }
    if (sms_string == SLEEP)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::SET_MODE, source_phone, WORK_MODE::SLEEP};
    }
    return DefaultCommandData{source_phone};
}

DefaultCommandData DefaultCommandParser::parse_set_alarm_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(ALARM));
    if (sms_string == ON)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::SET_ALARM, source_phone, ALARM_STATE::ON};
    }
    if (sms_string == OFF)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::SET_ALARM, source_phone, ALARM_STATE::OFF};
    }
    if (!sms_string.startsWith(SENSITY))
    {
        return DefaultCommandData{source_phone};
    }
    sms_string.substring(sms_string, sizeof(SENSITY));
    int result;
    if (!sms_string.toInt(result))
    {
        return DefaultCommandData{source_phone};
    }
    return DefaultCommandData{DEFAULT_COMMANDS::SET_ALARM_SENSITY, source_phone, result};

}

DefaultCommandData DefaultCommandParser::parse_set_low_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(LOW1));
    if (!sms_string.startsWith(BATTERY))
    {
        return DefaultCommandData{source_phone};
    }
    sms_string.substring(sms_string, sizeof(BATTERY));
    if (sms_string == ON)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::SET_LOW_BATTERY, source_phone, LOW_BATTERY_STATE::ON};
    }
    if (sms_string == OFF)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::SET_LOW_BATTERY, source_phone, LOW_BATTERY_STATE::OFF};
    }
    return DefaultCommandData{source_phone};
}

DefaultCommandData DefaultCommandParser::parse_get_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(GET));

    if (sms_string == BATTERY)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::GET_BATTERY, source_phone};
    }
    if (sms_string == TIME)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::GET_TIME, source_phone};
    }
    if (sms_string == SIGNAL)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::GET_SIGNAL, source_phone};
    }
    if (sms_string == ADMIN)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::GET_ADMIN, source_phone};
    }
    if (sms_string.startsWith(LOW1))
    {
        return parse_get_low_(sms_string, source_phone);
    }
    if (sms_string == ALARM)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::GET_ALARM, source_phone};
    }
    if (sms_string.startsWith(ALARM))
    {
        return parse_get_alarm_(sms_string, source_phone);
    }
    if (sms_string == MODE)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::GET_MODE, source_phone};
    }

    return DefaultCommandData{source_phone};

}

DefaultCommandData DefaultCommandParser::parse_get_alarm_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(ALARM));
    if (sms_string == SENSITY)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::GET_ALARM_SENSITY, source_phone};
    }
    return DefaultCommandData{source_phone};
}

DefaultCommandData DefaultCommandParser::parse_get_low_(SafeString& sms_string, Phone& source_phone)
{
    sms_string.substring(sms_string, sizeof(LOW1));
    if (sms_string == BATTERY)
    {
        return DefaultCommandData{DEFAULT_COMMANDS::GET_LOW_BATTERY, source_phone};
    }
    return DefaultCommandData{source_phone};
}