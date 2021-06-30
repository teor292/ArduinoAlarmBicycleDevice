#include "GPSDeviceState.h"

#if defined(GPS)

#include <Arduino.h>
#include "header.h"
#include "time_utils.h"

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

bool GPSDeviceBaseState::Activate()
{
    return false;
}

bool GPSDeviceBaseState::ResetActive()
{
    return false;
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
    duration_(ms_to_time(duration))
{}
    
bool GPSDeviceState::IsActive()
{
    if (0 == last_alarm_time_) return false;
    //auto current_time = millis();
    auto current_time = time();

    return current_time < last_alarm_time_ + duration_;
}

bool GPSDeviceState::Activate()
{
    bool was_active = IsActive();
    //last_alarm_time_ = millis();
    last_alarm_time_ = time();
    return !was_active;
}

bool GPSDeviceState::ResetActive()
{
    bool was_active = IsActive();
    last_alarm_time_ = 0;
    return was_active;
}

void GPSDeviceState::SetDuration(uint32_t duration)
{
    duration_ = ms_to_time(duration);
    PRINT("Duration: ");
    PRINTLN(duration);
}



bool GPSDeviceStateForce::IsActive()
{
    return active_;
}

bool GPSDeviceStateForce::Activate()
{
    auto old_state = active_;
    active_ = true;
    return !old_state;
}

bool GPSDeviceStateForce::ResetActive() 
{
    auto old_state = active_;
    active_ = false;
    return old_state;
}


#endif