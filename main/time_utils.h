#pragma once

#include <Arduino.h>
#if defined(__SAMD21G18A__)
#include "RTCMode0.h"
#endif

inline uint32_t time()
{
    #if defined(__SAMD21G18A__)
    return RTCMode0::Time();
    #else
    return millis();
    #endif
}