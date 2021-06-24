#include "GPSAutoStater.h"

#if defined(GPS)

#include <Arduino.h>

#define FORCE_POS 0
#define ALARM_POS 1
#define DEFAULT_POS 2
#define FORCE_PSM_POS 3

#define DEFAULT_RATE 5000

GPSAutoStater::GPSAutoStater(Stream& gps_stream, NonUbxCallback non_ubx_callback, WaitCallback wait_callback)
    : device_(gps_stream, non_ubx_callback, wait_callback),
    last_state_(GPS_DEVICE_WORK_MODE::CONTINOUS)
{
    //force state (by command -> get gps)
    //rate = 5, active time = 10 min
    states_.push_back(
        std::shared_ptr<GPSDeviceBaseState>(default_force_()));
    //alarm state (by vibro)
    //OFF, rate 5, active time = 3 min
    states_.push_back(
        std::shared_ptr<GPSDeviceBaseState>(default_alarm_()));
    //default states
    states_.push_back(
        std::shared_ptr<GPSDeviceBaseState>(default_alarm_()));

    //force state (by GPSManualPSM)
    states_.push_back(
        std::shared_ptr<GPSDeviceBaseState>(default_force_()));
}

GPSDeviceStateForce* GPSAutoStater::default_force_()
{
    return new GPSDeviceStateForce(GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::CONTINOUS));
}
GPSDeviceState* GPSAutoStater::default_alarm_()
{
    GPSAlarmSettings tmp;
    return new GPSDeviceState(GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::OFF), tmp.duration * 60UL * 1000UL);
}
GPSDeviceBaseState* GPSAutoStater::default_standart_()
{
    return new GPSDeviceBaseState(GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::OFF));
}

void GPSAutoStater::Initialize()
{
    //device go to continous mode
    device_.Initialize();
    //set rate fix
    set_rate_(DEFAULT_RATE);
    initialized_ = true;
}

GPS_ERROR_CODES GPSAutoStater::SetSettings(const GPSStateSettings& settings)
{
    if (!initialized_) return GPS_ERROR_CODES::NOT_INITIALIZED;
    

    auto fix_settings = get_device_states_settings_(settings.fix_settings);
    states_[DEFAULT_POS]->SetMode(fix_settings);
    auto alarm_settings = get_device_states_settings_(settings.alarm_settings);
    states_[ALARM_POS]->SetMode(alarm_settings);
    auto pt = reinterpret_cast<GPSDeviceState*>(states_[ALARM_POS].get());
    pt->SetDuration(settings.alarm_settings.GetDurationMs());

    auto new_mode = get_current_mode_();

    if (new_mode != last_state_)
    {
        return set_mode_device_(new_mode);
    }

    return GPS_ERROR_CODES::OK;

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

GPS_ERROR_CODES GPSAutoStater::Work(bool alarm)
{
    if (alarm)
    {
        states_[ALARM_POS]->Activate();
    }

    auto new_mode = get_current_mode_();
    if (new_mode == last_state_) return GPS_ERROR_CODES::OK;

    return set_mode_device_(new_mode);
}

GPS_ERROR_CODES GPSAutoStater::Force(GPS_STATER_FORCE force)
{
    switch (force)
    {
    case GPS_STATER_FORCE::BY_COMMAND:
        if (!states_[FORCE_POS]->Activate()) return GPS_ERROR_CODES::OK;
        break;
    case GPS_STATER_FORCE::BY_SERVICE:
        if (!states_[FORCE_PSM_POS]->Activate()) return GPS_ERROR_CODES::OK;
        break;
    }
    auto new_mode = get_current_mode_();
    if (new_mode == last_state_) return GPS_ERROR_CODES::OK;

    return set_mode_device_(new_mode);
}

GPS_ERROR_CODES GPSAutoStater::ResetForce(GPS_STATER_FORCE force)
{
    switch (force)
    {
    case GPS_STATER_FORCE::BY_COMMAND:
        if (!states_[FORCE_POS]->ResetActive()) return GPS_ERROR_CODES::OK;
        break;
    case GPS_STATER_FORCE::BY_SERVICE:
        if (!states_[FORCE_PSM_POS]->ResetActive()) return GPS_ERROR_CODES::OK;
        break;
    }
    auto new_mode = get_current_mode_();
    if (new_mode == last_state_) return GPS_ERROR_CODES::OK;

    return set_mode_device_(new_mode);
}

GPS_ERROR_CODES GPSAutoStater::set_mode_device_(const GPSDeviceStateSettings& mode)
{
    GPS_ERROR_CODES result = GPS_ERROR_CODES::NOT_SUPPORTED;
    switch (mode.GetMode())
    {
    case GPS_DEVICE_WORK_MODE::CONTINOUS:
        result = set_continous_mode_(mode.GetTime());
        break;
    case GPS_DEVICE_WORK_MODE::PSMCT:
    case GPS_DEVICE_WORK_MODE::PSMOO:
        result = set_psm_mode_(mode);
        break;
    case GPS_DEVICE_WORK_MODE::OFF:
        result = set_off_mode_();
        break;
    }
    if (GPS_OK(result))
    {
        last_state_ = mode;
    }
    return result;
}

GPS_ERROR_CODES GPSAutoStater::set_continous_mode_(uint32_t rate)
{
    auto result = device_.SetMode(GPS_DEVICE_WORK_MODE::CONTINOUS);
    if (!GPS_OK(result)) return result;

    return set_rate_(static_cast<uint16_t>(rate) * 1000);
}

GPS_ERROR_CODES GPSAutoStater::set_rate_(uint16_t time)
{
    if (current_rate_time_ == time) return GPS_ERROR_CODES::OK;
    UBX_CFG_RATE rate;
    rate.message.measRate = time;
    if (1000 > rate.message.measRate || 10000 < rate.message.measRate)
    {
        rate.message.measRate = 1000;
    }
    current_rate_time_ = rate.message.measRate;
    return device_.SetRate(rate);
}


GPS_ERROR_CODES GPSAutoStater::set_psm_mode_(const GPSDeviceStateSettings& mode)
{
    switch (mode.GetMode())
    {
    case GPS_DEVICE_WORK_MODE::PSMCT:
        return set_psmct_mode_(mode);
    case GPS_DEVICE_WORK_MODE::PSMOO:
        return set_psmoo_mode_(mode);
    }
    return GPS_ERROR_CODES::NOT_SUPPORTED;
}

GPS_ERROR_CODES GPSAutoStater::set_psmct_mode_(const GPSDeviceStateSettings& mode)
{
    if (!mode.GetNotEnterOff())
    {
        return GPS_ERROR_CODES::INVALID_ARGUMENT;
    }
    UBX_CFG_PM2 conf;


    conf.message.updatePeriod = DEFAULT_RATE;
    conf.message.searchPeriod = mode.GetSearchTimeMs();
    conf.message.doNotEnterOff = 1;

    auto result = device_.SetModeSettings(conf);
    if (!GPS_OK(result)) return result;

    result = set_rate_(DEFAULT_RATE);
    if (!GPS_OK(result)) return result;

    return device_.SetMode(GPS_DEVICE_WORK_MODE::PSMCT);
}

GPS_ERROR_CODES GPSAutoStater::set_psmoo_mode_(const GPSDeviceStateSettings& mode)
{
    UBX_CFG_PM2 conf;

    conf.message.updatePeriod = mode.GetTimeMs();
    //60s - 59m max
    if (60000UL >= conf.message.updatePeriod || 3600000UL <= conf.message.updatePeriod)
    {
        return GPS_ERROR_CODES::INVALID_ARGUMENT;
    }
    conf.message.searchPeriod = mode.GetSearchTimeMs();
    if (mode.GetNotEnterOff())
    {
        if (300000UL <= conf.message.updatePeriod)
        {
            return GPS_ERROR_CODES::INVALID_ARGUMENT;
        }
        conf.message.doNotEnterOff = 1;
    }
    if (300000UL >= conf.message.searchPeriod)
    {
        return GPS_ERROR_CODES::INVALID_ARGUMENT;
    }
    
    auto result = device_.SetModeSettings(conf);
    if (!GPS_OK(result)) return result;

    result = set_rate_(DEFAULT_RATE);
    if (!GPS_OK(result)) return result;

    return device_.SetMode(GPS_DEVICE_WORK_MODE::PSMCT);
}

 GPS_ERROR_CODES GPSAutoStater::set_off_mode_()
 {
     return device_.SetMode(GPS_DEVICE_WORK_MODE::OFF);
 }

GPSDeviceStateSettings GPSAutoStater::get_current_mode_() const
{
    GPSDeviceStateSettings result(GPS_DEVICE_WORK_MODE::OFF);
    for (auto& st : states_)
    {
        if (st->IsActive())
        {
            auto& current_mode = st->GetMode();
            if (current_mode > result)
            {
                result = current_mode;
            }
        }
    }
    return result;
}

void GPSAutoStater::reset_stater_settings_()
{
    states_[DEFAULT_POS].reset(default_standart_());
    states_[ALARM_POS].reset(default_alarm_());
    states_[FORCE_POS].reset(default_force_());
    states_[FORCE_PSM_POS].reset(default_force_());
    current_rate_time_ = 1000;
}

GPSDeviceStateSettings GPSAutoStater::get_device_states_settings_(const GPSFixSettings& settings)
{
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
    if (0 == settings.update_time)
    {
        return GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::OFF);
    }
    if (1 <= settings.update_time && 10 > settings.update_time)
    {
        return GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::CONTINOUS, settings.update_time);
    }
    if (10 <= settings.update_time && 60 > settings.update_time)
    {
        return GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::PSMCT, 5, true);
    }
    if (60 <= settings.update_time && 300 > settings.update_time)
    {
        return GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::PSMOO, settings.update_time, true);
    }
    if (300 <= settings.update_time && 600 > settings.update_time)
    {
        return GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::PSMOO, settings.update_time, false, 300000UL);
    }
    if (600 <= settings.update_time && 3600 > settings.update_time)
    {
        return GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::PSMOO, settings.update_time, false, 600000UL);
    }
    return GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::OFF);
}

GPSDeviceStateSettings GPSAutoStater::get_device_states_settings_(const GPSAlarmSettings& settings)
{
    switch (settings.mode)
    {      
    case GPS_ALARM_MODE::ON:
        return GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::PSMCT, 5, true);
    case GPS_ALARM_MODE::MAX:
        return GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::CONTINOUS);
    }

    return GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE::OFF);
}

#endif