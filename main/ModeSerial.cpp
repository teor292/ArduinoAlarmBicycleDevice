#include "ModeSerial.h"
#include <Arduino.h>
#include "header.h"

#define TIME_SLEEP 4000 //SIM800L sleep after 5000 ms, but use 4000 for reliability

#if defined(__SAMD21G18A__)

const int RXPin = 5, TXPin = 4;

//SoftwareSerial not work properly after wakeup (works only transmit)
//Perhaps it has something to do with timers
//So, create hardware uart
//(not use Serial1 because it will be used with gps module)
//using sercom2 -> disable I2C interface
ModeSerial::ModeSerial()
    : Uart( &sercom2, RXPin, TXPin, SERCOM_RX_PAD_1, UART_TX_PAD_0)
{}


#else

const int RXPin = 8, TXPin = 9;

ModeSerial::ModeSerial()
    : SoftwareSerial(RXPin, TXPin)
{}

#endif



void ModeSerial::SetMode(WORK_MODE mode)
{
    current_mode_ = mode;
    last_time_ = 0;
}

WORK_MODE ModeSerial::GetMode()
{
    return current_mode_;
}

bool ModeSerial::IsSleepMode()
{
    return WORK_MODE::SLEEP == current_mode_;
}

size_t ModeSerial::write(uint8_t c)
{
    awake_if_sleep_();
    return CUART::write(c);
}

size_t ModeSerial::write(const uint8_t *buffer, size_t size)
{
    awake_if_sleep_();
    return CUART::write(buffer, size);
}

void ModeSerial::ResetTime()
{
    last_time_ = 0;
}

void ModeSerial::awake_if_sleep_()
{
    if (WORK_MODE::STANDART == current_mode_) return;

    auto current_time = millis();
    if (0 != last_time_
        && current_time < last_time_ + TIME_SLEEP) return;
    PRINTLN("awake");
    CUART::write(' ');
    delay(150); //100 ms for awake, use 150 for reliability
    last_time_ = current_time;
}