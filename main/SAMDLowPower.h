#pragma once

#include "RTCMode0.h"

#if defined(__SAMD21G18A__)

//based on ArduinoLowPower library:
//https://github.com/arduino-libraries/ArduinoLowPower
class SAMDLowPower
{
    public:

        static void Initialize();
        static void SetAwakeTimeCallback(InterruptCallback callback);
        static bool SetAwakeCallback(uint8_t pin, InterruptCallback callback, int mode);
        static void UnsetAwakeCallback(uint8_t pin);
        static void SleepFor(uint32_t time);
        static void SleepTo(uint32_t time);
        static void Sleep();

    private:

        static void configure_gclk6_();
};

#endif