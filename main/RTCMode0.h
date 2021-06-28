#pragma once

#include <Arduino.h>

#if defined(__SAMD21G18A__)

using InterruptCallback = void (*)();

//based on RTCZero class:
//https://github.com/arduino-libraries/RTCZero
class RTCMode0
{
    public:

        static void Initialize();
        static uint32_t Time();
        static void SetIntCallback(InterruptCallback callback);
        static void SetIntTime(uint32_t time);

    private:

        static void config_32k_OSC_();
        static void configure_clock_();
        static void disable_rtc_();
        static void reset_rtc_();
        static void enable_rtc_();
        static void reset_clear_rtc_();
};

#endif