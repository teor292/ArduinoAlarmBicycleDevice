#include "gps_header.h"

#if defined(GPS)

#include "TextCommands.h"

void time_to_string(uint32_t time_seconds, SafeString& result)
{
    if (3600 > time_seconds && 0 == time_seconds % 3600UL)
    {
        result += time_seconds / 3600UL;
        result += 'h';
        return;
    }
    if (60 > time_seconds && 0 == time_seconds % 60UL)
    {
        result += time_seconds / 60UL;
        result += 'm';
        return;
    }
    result += time_seconds;
}

void GPSFixSettings::ToString(SafeString& result) const
{
    result += FIX;
    result += ' ';
    time_to_string(update_time, result);    
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

int GPSSettings::find_settings_(const SendSettings& settings) const
{
    auto cnt = send_settings_.size();
    for (int i = 0; i < cnt; ++i)
    {
        auto& send_settings = send_settings_[i];
        if (send_settings == settings)
        {
            return i;
        }
    }
    return -1;
}

void GPSSettings::AddOrUpdateSendSettings(const SendSettings& settings)
{
    auto index = find_settings_(settings);
    if (-1 == index)
    {
        if (send_settings_.max_size() == send_settings_.size())
        {
            send_settings_.remove(0);
        }
        send_settings_.push_back(settings);
    } 
    else
    {
        send_settings_[index] = settings;
    }
}

void GPSSettings::RemoveSendSettings(const SendSettings& settings)
{
    auto index = find_settings_(settings);
    if (-1 == index) return;
    send_settings_.remove(index);
}

void SendSettings::ToString(SafeString& result) const
{
    if (!result.isEmpty())
    {
        result += '\n';
    }
    if (SENDER_TYPE::SMS == send_data.type)
    {
        result += F("Phone: "); 
        result += send_data.values.phone.phone;
        result += '\n';
    }
    result += F("Send: ");
    time_to_string(data_.send_time, result);
    result += '\n';
    result += F("Valid: ");
    time_to_string(data_.valid_time, result);        
}



#endif