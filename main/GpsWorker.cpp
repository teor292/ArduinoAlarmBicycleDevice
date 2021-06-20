#include "GPSWorker.h"

#if defined(GPS)

namespace
{
    const char OK[] = "OK";
}

GPSWorker::GPSWorker(Stream& stream, Sms& sms, WaitCallback wait_callback)
    : gps_stream_(stream),
    sms_(sms),
    auto_stater_(stream, this, wait_callback),
    manual_psm_(auto_stater_, this),
    data_getter_(this),
    sms_send_manager_(sms, gps_)
{
    //senders_.push_back(&sms_sender_);
}

void GPSWorker::Read()
{
    while (gps_stream_.available())
    {
        gps_.encode((char)gps_stream_.read());
    }
}

void GPSWorker::Work(bool was_alarm)
{
    auto_stater_.Work(was_alarm);
    manual_psm_.Work();
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
    default:
        break;
    }
    /*
    SET_GPS_SMS_SEND, //sms like: set gps send sms me/phone 1800-86400/30m-1440m/1h-24h [age 5-3600/1m-60m/1h]
    GET_GPS_SMS_SEND, //sms like: get gps send sms
    GPS_RESET_SETTINGS, //sms: gps reset settings
    GPS_RESET_DEVICE, //sms: gps reset device
    */
}

void GPSWorker::get_gps_send_sms_(const GPSCommandData& command)
{

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
    auto old_vibro = settings_.state_settings.alarm_settings;
    settings_.state_settings.alarm_settings.mode = command.alarm_mode;
    settings_.state_settings.alarm_settings.duration = command.alarm_time;
    auto result = auto_stater_.SetSettings(settings_.state_settings);
    
    if (GPS_ERROR_CODES::OK == result)
    {
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
    data_getter_.RemoveFromWait(command.phone);
    send_ok_(command);
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
    sms_.SendSms("Wait max 10m");
    data_getter_.AddToWait(command.phone);
}

void GPSWorker::NonUBXSymbol(uint8_t c)
{
    gps_.encode(c);
}

bool GPSWorker::IsValidGPS(uint32_t valid_period_time)
{
    return gps_.location.age() < valid_period_time;
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