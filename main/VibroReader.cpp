#include "VibroReader.h"
#include "header.h"
#include <Arduino.h>

VibroReader::VibroReader(int input)
    : VIBRO_INPUT(input)
{
    pinMode(VIBRO_INPUT, INPUT); //vibro input
}

//inc count of change by 1
void VibroReader::ForceChange()
{
    if (!enabled_) return;
    ++current_count_changes_;
}
//read pin and inc if changed
void VibroReader::ReadChange()
{
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
}
//is current in alarming
bool VibroReader::IsAlarm()
{
    return current_count_changes_ > count_changes_per_second_;
}

void VibroReader::SetCountChanges(int count_changes_per_second)
{
    count_changes_per_second_ = count_changes_per_second;
}

void VibroReader::EnableAlarm(bool enable)
{
    enabled_ = enable;
}