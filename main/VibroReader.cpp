#include "VibroReader.h"
#include "header.h"
#include <Arduino.h>
#include "time_utils.h"

VibroReader::VibroReader(int input)
    : VIBRO_INPUT(input)
{
    pinMode(VIBRO_INPUT, INPUT); //vibro input
}

bool VibroReader::AddVibroCallback(AbstractVibroCallback* callback)
{
    if (callbacks_.size() == callbacks_.max_size()) return false;
    callbacks_.push_back(callback);
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
    if (!enabled_) return;
    //reset every one second
    auto current_time = time();
    if (current_time - last_millis_ > s_to_time(1))
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
    for (auto& callback : callbacks_)
    {
        callback->Alarm(current_count_changes_ > count_changes_per_second_);
    }
}

void VibroReader::SetCountChanges(int count_changes_per_second)
{
    count_changes_per_second_ = count_changes_per_second;
}

void VibroReader::EnableAlarm(bool enable)
{
    enabled_ = enable;
}

uint32_t VibroReader::NextNeccessaryDiffTime(uint32_t current_time)
{
    if (!enabled_) return ULONG_MAX;
    return 0;
}