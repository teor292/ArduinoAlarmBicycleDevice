#include "GPSAutoStater.h"

#include <Arduino.h>

#define FORCE_POS 0
#define ALARM_POS 1
#define DEFAULT_POS 2

GPSAutoStater::GPSAutoStater(Stream& gps_stream, NonUbxCallback non_ubx_callback, WaitCallback wait_callback)
    : device_(gps_stream, non_ubx_callback, wait_callback)
{
    //add states by priority
    //force state (by command -> get gps)
    states_.push_back(std::shared_ptr<GPSDeviceBaseState>(new GPSDeviceState(GPS_DEVICE_WORK_MODE::CONTINOUS, 300000UL)));
    //alarm state (by vibro)
    states_.push_back(std::shared_ptr<GPSDeviceBaseState>(new GPSDeviceState(GPS_DEVICE_WORK_MODE::CONTINOUS, 120000UL)));
    //default states
    states_.push_back(std::shared_ptr<GPSDeviceBaseState>(new GPSDeviceBaseState(GPS_DEVICE_WORK_MODE::CONTINOUS)));
}

void GPSAutoStater::Initialize(const GPSAllModeSettings& modes_settings)
{
    //device go to continous mode
    device_.Initialize();
    modes_settings_ = modes_settings;
    //set rate fix
    set_rate_(modes_settings_.continous_mode_settings.time_fix * 1000);
    initialized_ = true;
}

GPS_ERROR_CODES GPSAutoStater::SetModesSettings(const GPSAllModeSettings& modes_settings)
{
    if (!initialized_) return GPS_ERROR_CODES::NOT_INITIALIZED;
    auto mode = get_current_mode_();

    bool update = !check_equals_(mode, modes_settings);
    modes_settings_ = modes_settings;
    if (!update) return GPS_ERROR_CODES::OK;

    return set_mode_device_(mode);
}

GPS_ERROR_CODES GPSAutoStater::SetCurrentRegime(GPSRegimeSettings& settings)
{
    if (!initialized_) return GPS_ERROR_CODES::NOT_INITIALIZED;

    check_settings_(settings);

    const auto current_mode = get_current_mode_();
    current_settings_ = settings;
    states_[ALARM_POS]->SetMode(settings.mode_on_alarm);
    states_[DEFAULT_POS]->SetMode(settings.mode);
    const auto new_mode = get_current_mode_();

    if (current_mode != new_mode)
    {
        return set_mode_device_(new_mode);
    }

    return GPS_ERROR_CODES::OK;
}


GPSRegimeSettings GPSAutoStater::GetCurrentRegime()
{
    return current_settings_;
}

GPS_ERROR_CODES GPSAutoStater::ResetSettings()
{
    auto result = device_.ResetSettings();
    if (!GPS_OK(result)) return result;

    reset_stater_settings_();
    return result;
}

void GPSAutoStater::ResetDevice()
{
    device_.ResetDevice();

    reset_stater_settings_();
}

void GPSAutoStater::Work(bool alarm)
{
    auto current_mode = get_current_mode_();
    states_[ALARM_POS]->Active(alarm);
    auto new_mode = get_current_mode_();
    if (new_mode == current_mode) return;

    set_mode_device_(new_mode);

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

    return set_rate_(modes_settings_.continous_mode_settings.time_fix * 1000);
}

GPS_ERROR_CODES GPSAutoStater::set_rate_(uint16_t time)
{
    if (current_rate_time_ == time) return GPS_ERROR_CODES::OK;
    UBX_CFG_RATE rate;
    rate.message.measRate = time;
    if (rate.message.measRate < 1000 || rate.message.measRate > 10000)
    {
        rate.message.measRate = 1000;
    }
    current_rate_time_ = rate.message.measRate;
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

    result = set_rate_(modes_settings_.psmct_mode_settings.time_fix * 1000);
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

void GPSAutoStater::check_settings_(GPSRegimeSettings& settings)
{
    if (GPS_DEVICE_WORK_MODE::CONTINOUS != settings.mode_on_alarm
         && GPS_DEVICE_WORK_MODE::PSMCT != settings.mode_on_alarm
         && GPS_DEVICE_WORK_MODE::INVALID != settings.mode_on_alarm)
    {
        settings.mode_on_alarm = GPS_DEVICE_WORK_MODE::CONTINOUS;
    }
    if (GPS_DEVICE_WORK_MODE::INVALID == settings.mode)
    {
        settings.mode = GPS_DEVICE_WORK_MODE::CONTINOUS;
    }
}


bool GPSAutoStater::check_equals_(GPS_DEVICE_WORK_MODE mode, const GPSAllModeSettings& settings) const
{
    switch (mode)
    {
    case GPS_DEVICE_WORK_MODE::CONTINOUS:
        return modes_settings_.continous_mode_settings.DeviceEquals(settings.continous_mode_settings);
    case GPS_DEVICE_WORK_MODE::PSMCT:
        return modes_settings_.psmct_mode_settings.DeviceEquals(settings.psmct_mode_settings);
    case GPS_DEVICE_WORK_MODE::PSMOO:
        return modes_settings_.psmoo_mode_settings.DeviceEquals(settings.psmoo_mode_settings);
    }
    return true;
}

GPS_DEVICE_WORK_MODE GPSAutoStater::get_current_mode_() const
{
    GPS_DEVICE_WORK_MODE current_mode = GPS_DEVICE_WORK_MODE::INVALID;
    for (auto& st : states_)
    {
        if (st->IsActive())
        {
            current_mode = st->GetMode();
            if (GPS_DEVICE_WORK_MODE::INVALID == current_mode) continue;
            break;
        }
    }
    if (GPS_DEVICE_WORK_MODE::INVALID == current_mode)
    {
        current_mode = states_[DEFAULT_POS]->GetMode();
    }
    return current_mode;
}

void GPSAutoStater::reset_stater_settings_()
{
    current_settings_ = GPSRegimeSettings();
    modes_settings_ = GPSAllModeSettings();
    current_rate_time_ = 1000;

    for (auto& s : states_)
    {
        s->ForceResetActive();
    }
}