#pragma once

#include "BatteryChecker.h"

void go_to_sleep(BatteryChecker& checker, uint32_t current_time, uint32_t sleep_for_time);
void int0_func();
extern volatile bool f_extern_interrupt;
