#include "GPSAutoStater.h"

#include <Arduino.h>

#define MIN_ALARM_TIME 120000

GPSAutoStater::GPSAutoStater(Stream& gps_stream, NonUbxCallback non_ubx_callback, WaitCallback wait_callback)
    : device_(gps_stream, non_ubx_callback, wait_callback)
{
}

void GPSAutoStater::Initialize(const GPSAllModeSettings& modes_settings)
{
    device_.Initialize();
    modes_settings_ = modes_settings;
    initialized_ = true;
}

GPS_ERROR_CODES GPSAutoStater::SetCurrentRegime(GPSRegimeSettings& settings)
{
    if (!initialized_) return GPS_ERROR_CODES::NOT_INITIALIZED;

    check_alarm_settings_(settings);

    if (is_in_alarm_)
    {
        bool update = current_settings_.mode_on_alarm != settings.mode_on_alarm;
        current_settings_ = settings;
        if (update)
        {
            return set_current_mode_settings_();
        }
        return GPS_ERROR_CODES::OK;
    }

    bool update = settings.mode != current_settings_.mode;

    current_settings_ = settings;

    if (update)
    {
        return set_current_mode_settings_();
    }
    return GPS_ERROR_CODES::OK;
}


GPSRegimeSettings GPSAutoStater::GetCurrentRegime()
{
    return current_settings_;
}

GPS_ERROR_CODES GPSAutoStater::ResetSettings()
{
    return device_.ResetSettings();
}

void GPSAutoStater::ResetDevice()
{
    return device_.ResetDevice();
}

void GPSAutoStater::Work(bool alarm)
{
    if (alarm)
    {
        last_alarm_time_ = millis();
        if (!is_in_alarm_)
        {
            is_in_alarm_ = true;
            set_current_mode_settings_();
        }
    } else
    {
        if (is_in_alarm_)
        {
            auto current_time = millis();
            if (current_time - last_alarm_time_ < MIN_ALARM_TIME) return;
            is_in_alarm_ = false;
            set_current_mode_settings_();
        }
    }
}


GPS_ERROR_CODES GPSAutoStater::set_current_mode_settings_()
{
    GPS_DEVICE_WORK_MODE mode = is_in_alarm_ ? current_settings_.mode_on_alarm : current_settings_.mode;
    return set_mode_device_(mode);
}

GPS_ERROR_CODES GPSAutoStater::set_mode_device_(GPS_DEVICE_WORK_MODE mode)
{
    switch (mode)
    {
    case GPS_DEVICE_WORK_MODE::CONTINOUS:
        return set_continous_mode_();
    case GPS_DEVICE_WORK_MODE::PSMCT:
        return set_psmct_mode_();
    case GPS_DEVICE_WORK_MODE::PSMOO:
        return set_psmoo_mode_();
    case GPS_DEVICE_WORK_MODE::SOFTWARE_OFF:
        return set_off_mode_();
    }
    return GPS_ERROR_CODES::NOT_SUPPORTED;
}

GPS_ERROR_CODES GPSAutoStater::set_continous_mode_()
{
    auto result = device_.SetMode(GPS_DEVICE_WORK_MODE::CONTINOUS);
    if (!GPS_OK(result)) return result;

    UBX_CFG_RATE rate;
    rate.message.measRate = modes_settings_.continous_mode_settings.time_fix * 1000;
    if (rate.message.measRate < 500 || rate.message.measRate > 10000)
    {
        rate.message.measRate = 1000;
    }
    return device_.SetRate(rate);
}

GPS_ERROR_CODES GPSAutoStater::set_psmct_mode_()
{
    UBX_CFG_PM2 conf;
    conf.message = UBX_CFG_PM2_BASE_::GetDefaultPCMT();
    conf.message.updatePeriod = modes_settings_.psmct_mode_settings.time_fix * 1000;
    if (conf.message.updatePeriod < 5000 || conf.message.updatePeriod > 10000)
    {
        conf.message.updatePeriod = 5000;
    }
    auto result = device_.SetModeSettings(conf);
    if (!GPS_OK(result)) return result;

    return device_.SetMode(GPS_DEVICE_WORK_MODE::PSMCT);
}

GPS_ERROR_CODES GPSAutoStater::set_psmoo_mode_()
{
    UBX_CFG_PM2 conf;
    conf.message = UBX_CFG_PM2_BASE_::GetDefaultPCMOO();
    conf.message.updatePeriod = modes_settings_.psmoo_mode_settings.time_fix * 1000;
    if (conf.message.updatePeriod < 10000 || conf.message.updatePeriod > 60000)
    {
        conf.message.updatePeriod = 10000;
    }
    auto result = device_.SetModeSettings(conf);
    if (!GPS_OK(result)) return result;

    return device_.SetMode(GPS_DEVICE_WORK_MODE::PSMOO);
}

 GPS_ERROR_CODES GPSAutoStater::set_off_mode_()
 {
     return device_.SetMode(GPS_DEVICE_WORK_MODE::SOFTWARE_OFF);
 }

void GPSAutoStater::check_alarm_settings_(GPSRegimeSettings& settings)
{
    if (GPS_DEVICE_WORK_MODE::CONTINOUS != settings.mode_on_alarm
         && GPS_DEVICE_WORK_MODE::PSMCT != settings.mode_on_alarm)
    {
        settings.mode_on_alarm = GPS_DEVICE_WORK_MODE::CONTINOUS;
    }
}
