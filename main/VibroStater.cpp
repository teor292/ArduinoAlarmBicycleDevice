#include "VibroStater.h"
#include "header.h"
#include <Arduino.h>

#define ALARM_TIME 2 * 60 * 1000L //1 sms per two minutes

VibroStater::VibroStater(int input)
    : VIBRO_INPUT(input),
    count_changes_per_second_(0),
    current_count_changes_(0),
    last_millis_(0),
    last_alarm_time_(0),
    previous_state_(0),
    current_state_(0),
    enabled_(1)
{
    pinMode(VIBRO_INPUT, INPUT); //vibro input
}

bool VibroStater::Update()
{
    if (!enabled_) return false;
    //reset every one second
    auto current_time = millis();
    if (current_time - last_millis_ > 1000)
    {
        last_millis_ = current_time;
        current_count_changes_ = 0;
    }

    previous_state_ = current_state_;
    current_state_ = static_cast<unsigned char>(digitalRead(VIBRO_INPUT)); 

    if (current_state_ != previous_state_)
    {
        PRINTLN("VIBRO");
        ++current_count_changes_;
    }

    if (current_count_changes_ > count_changes_per_second_
        && (current_time > last_alarm_time_ + ALARM_TIME
            || 0 == last_alarm_time_))
    {
        last_alarm_time_ = current_time;
        return true;
    }
    return false;

}

void VibroStater::SetCountChanges(unsigned int count_changes_per_second)
{
    count_changes_per_second_ = count_changes_per_second;
}

void VibroStater::EnableAlarm(unsigned char enable)
{
    enabled_ = enable;
    last_alarm_time_ = 0;
}