#pragma once

#if defined(__SAMD21G18A__)

#include <Uart.h>
using CUART = Uart;

#else

#include <SoftwareSerial.h>
using CUART = SoftwareSerial;

#endif