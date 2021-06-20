#pragma once

#include "gps_define.h"


#if defined(GPS)

#include "gps_consts.h"

#include <stdint.h>
#include <SafeString.h>
#include "TextCommands.h"
#include <Array.h>
#include "Phone.h"

void time_to_string(uint32_t time_seconds, SafeString& result);

//define work mode of gps module
enum class GPS_DEVICE_WORK_MODE
{
    CONTINOUS = 0, //default, no energy saving
    PSMCT, //short update period (< 10 seconds)
    PSMOO, //long update (> 10 second)
    OFF, //OFF by command || power,
    LAST = OFF //service variable
};

struct GPSFixSettings
{
    //fix gps coordinate interval, in seconds
    //0 - disabled (OFF mode)
    //1 - 9 CONTINOUS mode (rate 1 - 9)
    //10 - 59 - PSMCT (ack rate 5, update 5, doNotEnterOff flag enable)
    //60 - 299 - PSMOO (ack rate 5, doNotEnterOff flag enable)
    //300 - 599 - PSMOO (ack rate 5, searchPeriod 5 minutes)
    //600s - 59 min - PSMOO (ack rate 5, searchPeriod 10 minutes)
    //60 min - 24 h - manual (awake after time in CONTINOUS for 10 minutes,
    //if not found -> sleep: 
    // >=240 min = 2 h
    // >= 120 min = 1 h
    // >= 60 min = 30 min)
    uint32_t update_time{0}; 

    uint32_t UpdateTimeMS() const
    {
        return update_time * 1000UL;
    }

    void ToString(SafeString& result) const;
};

enum class GPS_ALARM_MODE
{
    OFF = 0, //disabled change state on alarm
    ON = 1, //enabled in PSCMT, update - 5 seconds, doNotEnterOff flag enable
    //if already in PSMCT with doNotEnterOff flag enabled || continous mode -> do nothing
    MAX = 2 //enabled in continous mode, rate 5
    //if already in continous with rate 1-5 -> do nothing
};

//settings how to work gps if was alarm
struct GPSAlarmSettings
{
    GPS_ALARM_MODE mode{GPS_ALARM_MODE::OFF};
    //duration of 'mode' after last alarm, default - 3 min
    uint8_t duration{180};

    uint32_t GetDurationMs() const
    {
        return 1000UL * duration;
    }

    void Check()
    {
        if (60 > duration || 600 < duration)
        {
            duration = 180;
        }
    }

    void ToString(SafeString& result) const;
};

//convience struct for settings
struct GPSStateSettings
{
    GPSFixSettings fix_settings;
    GPSAlarmSettings alarm_settings;
};

enum class SENDER_TYPE
{
    SMS = 0
};

//union for sending gps data identifier (currently - phone only (sms))
struct SenderData
{
    SENDER_TYPE type;
    union SenderValue
    {
        SenderValue()
        {
            memset(&phone, 0, sizeof(phone));
        }
        Phone phone;
    } values;
    
    bool operator == (const SenderData& data) const
    {
        if (type != data.type) return false;
        if (SENDER_TYPE::SMS == type)
        {
            return values.phone == data.values.phone;
        }
        return false;
    }

    SenderData& operator = (const SenderData& data)
    {
        type = data.type;
        if (SENDER_TYPE::SMS == type)
        {
            values.phone = data.values.phone;
        }
        return *this;
    }
    
};

struct SendSettingData
{
        //seconds, valid values depends on sender type
        //0 - off
        //sms: 1800-86400
        uint32_t send_time{0};
        //the time period (seconds) at which the coordinates are considered valid
        uint32_t valid_time{0};

        uint32_t GetSendTimeMs() const
        {
            return send_time * 1000UL;
        }

        uint32_t GetValidTimeMs() const
        {
            return valid_time * 1000UL;
        }
};



class SendSettings
{
    public:

        SenderData send_data;

        void SetSendTime(uint32_t send_time);
        void SetValidTime(uint32_t valid_time)
        {
            data_.valid_time = valid_time;
        }

        uint32_t GetSendTimeMs() const
        {
            return data_.GetSendTimeMs();
        }

        uint32_t GetValidTimeMs() const
        {
            return data_.GetValidTimeMs();
        }

        const SendSettingData& Data() const
        {
            return data_;
        }

        bool operator == (const SendSettings& settings) const
        {
            return send_data == settings.send_data;
        }

        SendSettings& operator = (const SendSettings& settings)
        {
            send_data = settings.send_data;
            data_ = settings.data_;
            return *this;
        }

        void ToString(SafeString& result) const;


    private:
        SendSettingData data_;
};


class GPSSettings
{
    public:

        GPSSettings();
        void Save();

        GPSStateSettings state_settings;

        void AddOrUpdateSendSettings(const SendSettings& settings);

        void RemoveSendSettings(const SendSettings& settings);
        
        const Array<SendSettings, MAX_SMS_SENDERS>& GetSendSettings() const
        {
            return send_settings_;
        }
    private:

        Array<SendSettings, MAX_SMS_SENDERS> send_settings_;

        int find_settings_(const SendSettings& settings) const;
};

#endif