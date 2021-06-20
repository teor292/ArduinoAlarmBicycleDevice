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


void SendSettings::SetSendTime(uint32_t send_time)
{
    data_.send_time = send_time;
    if (10 > send_time)
    {
        data_.valid_time = 5;
    }
    else if (60 > send_time)
    {
        data_.valid_time = 10;
    }
    else if (300 > send_time)
    {
        data_.valid_time = 60;
    }
    else if (600 > send_time)
    {
        data_.valid_time = 300;
    }
    else if (3600 > send_time)
    {
        data_.valid_time = 600;
    }
    else
    {
        data_.valid_time = 3600;
    }
}


GPSSettings::GPSSettings()
{}

void GPSSettings::Save()
{
    //TODO
}

void GPSSettings::AddOrUpdateSendSettings(const SendSettings& settings)
{
    SendSettings* founded = nullptr;
    for (auto& send_setting : send_settings_)
    {
        if (send_setting == settings)
        {
            founded = &send_setting;
            break;
        }
    }
    if (nullptr == founded)
    {
        if (send_settings_.max_size() == send_settings_.size())
        {
            send_settings_.remove(0);
        }
        send_settings_.push_back(settings);
    } 
    else
    {
        *founded = settings;
    }
}



#endif