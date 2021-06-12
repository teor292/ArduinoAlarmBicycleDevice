#include "GPSDeviceState.h"

#if defined(GPS)

#include <Arduino.h>

GPSDeviceBaseState::GPSDeviceBaseState(GPS_DEVICE_WORK_MODE mode)
    : mode_(mode)
{
}

GPSDeviceBaseState::~GPSDeviceBaseState()
{}

bool GPSDeviceBaseState::IsActive()
{
    return true;
}

void GPSDeviceBaseState::Active(bool active)
{
}

void GPSDeviceBaseState::ForceResetActive()
{
}

GPS_DEVICE_WORK_MODE GPSDeviceBaseState::GetMode() const
{
    return mode_;
}

void GPSDeviceBaseState::SetMode(GPS_DEVICE_WORK_MODE mode)
{
    mode_ = mode;
}

GPSDeviceState::GPSDeviceState(GPS_DEVICE_WORK_MODE mode, unsigned long duration)  
    : GPSDeviceBaseState(mode),
    duration_(duration)
{}
    
bool GPSDeviceState::IsActive()
{
    if (0 == last_alarm_time_) return false;
    auto current_time = millis();

    return current_time < last_alarm_time_ + duration_;
}

void GPSDeviceState::Active(bool active)
{
    if (!active) return;
    last_alarm_time_ = millis();
}

void GPSDeviceState::ForceResetActive()
{
    last_alarm_time_ = 0;
}

#endif