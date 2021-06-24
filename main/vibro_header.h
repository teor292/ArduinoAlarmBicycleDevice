#pragma once


//when changed alarm settings in gps or alarm by sms -> call this callback
using VibroAlarmChangeCallback = void (*)(bool alarm_enable);