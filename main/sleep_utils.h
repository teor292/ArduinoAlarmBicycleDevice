#pragma once

#include "BatteryChecker.h"

void go_to_sleep(BatteryChecker& checker);

extern volatile bool f_extern_interrupt;
