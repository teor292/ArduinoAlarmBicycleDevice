#pragma once

#include <Arduino.h>
#if defined(__SAMD21G18A__)
#include "RTCMode0.h"
#endif

//for samd -> return abstract time
//use abstract time instead of real
//This is necessary because rtc does not issue a tick every millisecond, 
//you can make it give out tick every second, 
//but then you need to translate the entire code into second intervals. 
//In this case, if you use milliseconds as a measure of seconds, 
//then a problem arises: an overflow of a millisecond in 49 days will not be an overflow of seconds 
//and something will definitely break.
inline uint32_t time()
{
    #if defined(__SAMD21G18A__)
    //for samd it is 1/4 second
    return RTCMode0::Time();
    #else
    //for nano and others it is ms
    return millis();
    #endif
}

inline uint32_t time_to_s(uint32_t time)
{
    #if defined(__SAMD21G18A__)
    return (time >> 2);
    #else
    return time / 1000UL;
    #endif
}

inline uint32_t time_to_ms(uint32_t time)
{
    #if defined(__SAMD21G18A__)
    return time * 250UL;
    #else
    return time;
    #endif
}

inline uint32_t time_to_ms_check(uint32_t time)
{
    #if defined(__SAMD21G18A__)
    if (time >= 0xFFFFFFFF / 250UL) return ULONG_MAX;
    return time * 250UL;
    #else
    return time;
    #endif
}

inline uint32_t s_to_time(uint32_t s)
{
    #if defined(__SAMD21G18A__)
    return (s << 2);
    #else
    return s * 1000UL;
    #endif

}

inline uint32_t ms_to_time(uint32_t s)
{
    #if defined(__SAMD21G18A__)
    return s / 250UL;
    #else
    return s;
    #endif

}

//A function for determining how long the microcontroller should wake up from sleep mode.
uint32_t get_next_diff_time(uint32_t last_time, uint32_t diff_next_time, uint32_t current_time);