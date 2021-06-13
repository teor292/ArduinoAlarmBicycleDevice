#include "GPSDeviceState.h"

#if defined(GPS)

#include <Arduino.h>

GPSDeviceBaseState::GPSDeviceBaseState(const GPSDeviceStateSettings& mode)
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

const GPSDeviceStateSettings& GPSDeviceBaseState::GetMode() const
{
    return mode_;
}

void GPSDeviceBaseState::SetMode(const GPSDeviceStateSettings& mode)
{
    mode_ = mode;
}

GPSDeviceState::GPSDeviceState(const GPSDeviceStateSettings& mode, uint32_t duration)  
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

void GPSDeviceState::SetDuration(uint32_t duration)
{
    duration_ = duration;
}



void GPSDeviceStateForce::Active(bool active)
{
    if (!active) return;
    if (IsActive()) return;
    last_alarm_time_ = millis();
}


#endif