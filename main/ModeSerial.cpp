#include "ModeSerial.h"
#include <Arduino.h>
#include "header.h"

#define TIME_SLEEP 4000 //SIM800L sleep after 5000 ms, but use 4000 for reliability

void ModeSerial::SetMode(WORK_MODE mode)
{
    current_mode_ = mode;
    last_time_ = 0;
}

WORK_MODE ModeSerial::GetMode()
{
    return current_mode_;
}

size_t ModeSerial::write(uint8_t c)
{
    awake_if_sleep_();
    return SoftwareSerial::write(c);
}

size_t ModeSerial::write(const uint8_t *buffer, size_t size)
{
    awake_if_sleep_();
    return SoftwareSerial::write(buffer, size);
}

void ModeSerial::awake_if_sleep_()
{
    if (WORK_MODE::STANDART == current_mode_) return;

    auto current_time = millis();
    if (current_time < last_time_ + TIME_SLEEP) return;
    PRINTLN("awake");
    SoftwareSerial::write(' ');
    delay(150); //100 ms for awake, use 150 for reliability
    last_time_ = current_time;
}