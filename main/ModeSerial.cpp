#include "ModeSerial.h"

#define TIME_SLEEP 4000 //SIM800L sleep after 5000 ms, but use 4000 for reliability

void ModeSerial::SetMode(WORK_MODE mode)
{
    current_mode_ = mode;
    last_time_ = 0;
}

size_t ModeSerial::write(const uint8_t *buffer, size_t size)
{
    auto current_time = millis();
    if (current_time_ < last_time_ + TIME_SLEEP)
    {
        return SoftwareSerial::write(buffer, size);
    }
    write(' ');
    delay(150); //100 ms for awaik, use 150 for reliability
    last_time_ = current_time_;
    return SoftwareSerial::write(buffer, size);
}