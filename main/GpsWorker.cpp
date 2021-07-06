#include "GPSWorker.h"

#if defined(GPS)

namespace
{
    const char OK[] = "OK";
}

GPSWorker::GPSWorker(Stream& stream, Sms& sms, WaitCallback wait_callback,
    VibroAlarmChangeCallback alarm_change_callback)
    : gps_stream_(stream),
    sms_(sms),
    auto_stater_(stream, this, wait_callback),
    manual_psm_(auto_stater_, this),
    data_getter_(this),
    sms_send_manager_(sms, gps_),
    alarm_change_callback_(alarm_change_callback)
{
  
}

void GPSWorker::Initialize()
{
    auto_stater_.Initialize();
    auto result = auto_stater_.SetSettings(settings_.state_settings);
    if (GPS_ERROR_CODES::OK != result)
    {
        PRINT("ERROR stater set settings: ");
        PRINTLN((int)result);
    }
    else
    {
        PRINTLN("Stater set settings OK");
    }
    manual_psm_.UpdateSettings(settings_.state_settings.fix_settings);
    vibro_stater_.EnableAlarm(IsAlarmEnabled());

}

void GPSWorker::Read()
{
    while (gps_stream_.available())
    {
        char c = (char)gps_stream_.read();
        PRINT(c);
        gps_.encode(c);
    }
}

bool GPSWorker::IsAlarmEnabled() const
{
    return GPS_ALARM_MODE::OFF != settings_.state_settings.alarm_settings.mode;
}

void GPSWorker::Work()
{
    Read();
    
    manual_psm_.Work();
    data_getter_.Work();

    if (data_getter_.IsActive())
    {
        //send error codes to admin???
        auto_stater_.Force(GPS_STATER_FORCE::BY_COMMAND);
    }
    else
    {
        auto_stater_.ResetForce(GPS_STATER_FORCE::BY_COMMAND);
    }
    auto_stater_.Work(vibro_stater_.IsAlarm());
    sms_send_manager_.Work();
}

uint32_t GPSWorker::NextNeccessaryDiffTime(uint32_t current_time)
{
    //if current state is not off -> do not sleep
    //although gps device can sleep in PSMOO mode but
    //currently awake microcontroller from gps data is not implemented
    if (GPS_DEVICE_WORK_MODE::OFF != auto_stater_.GetMode()) return 0;

    //calculate diff time with considering sms send time
    auto diff_time_by_sms_send = sms_send_manager_.GetNextDiffTime(current_time);
    auto psm_diff_time = manual_psm_.NextDiffAwakeTime(current_time);
    return diff_time_by_sms_send < psm_diff_time ? diff_time_by_sms_send : psm_diff_time;
}

void GPSWorker::PerformCommand(const GPSCommandData& command)
{
    switch (command.cmd)
    {
    case GPS_COMMANDS::GET_GPS:
        get_gps_(command);
        break;
    case GPS_COMMANDS::GET_GPS_RESET:
        get_gps_reset_(command);
        break;
    case GPS_COMMANDS::GET_LAST_GPS:
        get_last_gps_(command);
        break;
    case GPS_COMMANDS::SET_GPS_FIX:
        set_gps_fix_(command);
        break;
    case GPS_COMMANDS::GET_GPS_FIX:
        get_gps_fix_(command);
        break;
    case GPS_COMMANDS::SET_GPS_VIBRO:
        set_gps_vibro_(command);
        break;
    case GPS_COMMANDS::GET_GPS_VIBRO:
        get_gps_vibro_(command);
        break;
    case GPS_COMMANDS::SET_GPS_SMS_SEND:
        set_gps_send_sms_(command);
        break;
    case GPS_COMMANDS::GET_GPS_SMS_SEND:
        get_gps_send_sms_(command);
        break;
    case GPS_COMMANDS::SET_GPS_REMOVE_SMS_SEND:
        set_gps_send_sms_remove_(command);
        break;
    case GPS_COMMANDS::GPS_RESET_SETTINGS:
        gps_reset_settings_(command);
        break;
    case GPS_COMMANDS::GPS_RESET_DEVICE:
        gps_reset_device_(command);
        break;
    default:
        send_error_(command, GPS_ERROR_CODES::INVALID_ARGUMENT);
        break;
    }
}

void GPSWorker::gps_reset_device_(const GPSCommandData& command)
{
    auto_stater_.ResetDevice();
    auto result = auto_stater_.SetSettings(settings_.state_settings);
    
    if (GPS_ERROR_CODES::OK == result)
    {
        settings_.Save();
        send_ok_(command);
        return;
    }
    settings_.state_settings = GPSStateSettings{};
    settings_.Save();
    send_error_(command, result);
}

void GPSWorker::gps_reset_settings_(const GPSCommandData& command)
{
    settings_ = GPSSettings{};
    settings_.Save();
   auto stater_result = auto_stater_.ResetSettings();
   manual_psm_.UpdateSettings(settings_.state_settings.fix_settings);
   data_getter_.Reset();
   sms_send_manager_.Reset();
   if (GPS_ERROR_CODES::OK == stater_result)
   {
       send_ok_(command);
   } else
   {
        createSafeString(tmp, 50);
        tmp = F("Reset ok, but stater return: ");
        tmp += static_cast<int>(stater_result);
        sms_.SendSms(tmp.c_str());
   }
}

void GPSWorker::set_gps_send_sms_remove_(const GPSCommandData& command)
{
    sms_.SetPhone(command.phone.phone);
    SendSettings settings;
    settings.send_data.type = SENDER_TYPE::SMS;
    settings.send_data.values.phone = command.dst_phone;

    if (!sms_send_manager_.RemoveSender(settings))
    {
        createSafeString(tmp, 20);
        tmp += F("Phone not found");
        sms_.SendSms(tmp.c_str());
        return;
    }
    settings_.RemoveSendSettings(settings);
    send_ok_(command);
}

void GPSWorker::get_gps_send_sms_(const GPSCommandData& command)
{
    sms_.SetPhone(command.phone.phone);
    createSafeString(tmp, 160);
    auto& sender_settings = settings_.GetSendSettings();
    for (auto &s : sender_settings)
    {
        s.ToString(tmp);
    }
    if (tmp.isEmpty())
    {
        tmp = F("No settings");
    }
    sms_.SendSms(tmp.c_str());
}

void GPSWorker::set_gps_send_sms_(const GPSCommandData& command)
{
    SendSettings settings;
    settings.send_data.type = SENDER_TYPE::SMS;
    settings.send_data.values.phone = command.dst_phone;
    settings.SetSendTime(command.update_time);
    if (0 != command.age_time)
    {
        settings.SetValidTime(command.age_time);
    }
    if (0 != command.interval_not_send_time)
    {
        settings.SetIntervalTimes(command.interval_delay_time, command.interval_not_send_time);
    }
    settings_.AddOrUpdateSendSettings(settings);
    settings_.Save();
    sms_send_manager_.AddOrUpdateSender(settings);
    send_ok_(command);
}

void GPSWorker::get_gps_vibro_(const GPSCommandData& command)
{
    sms_.SetPhone(command.phone.phone);
    createSafeString(tmp, 20);
    settings_.state_settings.alarm_settings.ToString(tmp);
    sms_.SendSms(tmp.c_str());
}

void GPSWorker::set_gps_vibro_(const GPSCommandData& command)
{
    auto old_alarm = IsAlarmEnabled();
    auto old_vibro = settings_.state_settings.alarm_settings;
    settings_.state_settings.alarm_settings.mode = command.alarm_mode;
    auto alarm_time = command.alarm_time;
    if (0 == alarm_time)
    {
        GPSAlarmSettings tmp;
        alarm_time = tmp.duration;
    }
    settings_.state_settings.alarm_settings.duration = alarm_time;
    auto result = auto_stater_.SetSettings(settings_.state_settings);
    
    if (GPS_ERROR_CODES::OK == result)
    {
        auto new_alarm = IsAlarmEnabled();
        if (old_alarm != new_alarm)
        {
            alarm_change_callback_(new_alarm);
        }
        vibro_stater_.EnableAlarm(new_alarm);
        settings_.Save();
        send_ok_(command);
        return;
    }
    settings_.state_settings.alarm_settings = old_vibro;
    auto_stater_.SetSettings(settings_.state_settings);
    send_error_(command, result);
}

void GPSWorker::get_gps_fix_(const GPSCommandData& command)
{
    sms_.SetPhone(command.phone.phone);
    createSafeString(tmp, 20);
    settings_.state_settings.fix_settings.ToString(tmp);
    sms_.SendSms(tmp.c_str());
}

void GPSWorker::set_gps_fix_(const GPSCommandData& command)
{
    auto old_fix = settings_.state_settings.fix_settings;
    settings_.state_settings.fix_settings.update_time = command.update_time;
    auto result = auto_stater_.SetSettings(settings_.state_settings);
    if (GPS_ERROR_CODES::OK == result)
    {
        manual_psm_.UpdateSettings(settings_.state_settings.fix_settings);
        settings_.Save();
        send_ok_(command);
        return;
    }
    settings_.state_settings.fix_settings = old_fix;
    auto_stater_.SetSettings(settings_.state_settings);
    send_error_(command, result);
}

void GPSWorker::get_last_gps_(const GPSCommandData& command)
{
    SenderData data;
    data.values.phone = command.phone;
    GPSSmsSender::SendGPS(sms_, gps_, data, 0);
}

void GPSWorker::get_gps_reset_(const GPSCommandData& command)
{
    auto result = data_getter_.RemoveFromWait(command.phone);
    if (result)
    {
        send_ok_(command);
    }
    else
    {
        sms_.SendSms("Not found");
    }

}

void GPSWorker::send_ok_(const GPSCommandData& command)
{
    sms_.SetPhone(command.phone.phone);
    sms_.SendSms(OK);
}

void GPSWorker::send_error_(const GPSCommandData& command, GPS_ERROR_CODES code)
{
    sms_.SetPhone(command.phone.phone);
    createSafeString(tmp, 15);
    tmp = F("Error: ");
    tmp += static_cast<int>(code);
    sms_.SendSms(tmp.c_str());
}

void GPSWorker::get_gps_(const GPSCommandData& command)
{
    if (IsValidGPS(60000UL))
    {
        Send(command.phone);
        return;
    }
    sms_.SetPhone(command.phone.phone);

    if (!data_getter_.AddToWait(command.phone))
    {
        sms_.SendSms("GPS age > 1m, max phones wait reached");
    } else
    {
        sms_.SendSms("Wait max 10m");
    }
}

void GPSWorker::NonUBXSymbol(uint8_t c)
{
    gps_.encode(c);
}

bool GPSWorker::IsValidGPS(uint32_t valid_period_time_ms)
{
    return gps_.location.age() < valid_period_time_ms;
}

bool GPSWorker::CheckAge(uint32_t valid_time)
{
    return IsValidGPS(valid_time);
}

void GPSWorker::Send(const Phone& phone, bool valid)
{
    SenderData data;
    data.values.phone = phone;
    GPSSmsSender::SendGPS(sms_, gps_, data, valid ? 0 : static_cast<uint32_t>(ULONG_MAX));
}

#endif