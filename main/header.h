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

#if defined(__SAMD21G18A__)

#define AWAKE_SIM800_PIN 9
#define VIBRO_PIN 8

#else

#define AWAKE_SIM800_PIN 2
#define VIBRO_PIN 5

#endif

#define PHONE_BUF_LENGTH 19
#define SMS_BUF 255