#pragma once

#define DEBUG

#ifdef DEBUG

#include <Arduino.h>

#define PRINT(x) Serial.print((x))
#define PRINTLN(x) Serial.println((x))
#define WRITE(x) Serial.write((x))

#else

#define PRINT(x)
#define PRINTLN(x)
#define WRITE(x)

#endif
