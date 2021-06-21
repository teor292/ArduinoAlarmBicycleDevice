#pragma once

#include "Phone.h"
#include "work_mode.h"

enum class DEFAULT_COMMANDS
{
    GET_BATTERY, //sms: get battery
    GET_TIME, //sms: get time
    GET_SIGNAL, //sms: get signal
    SET_ADMIN, //sms: set admin
    GET_ADMIN, //sms: get admin
    SET_LOW_BATTERY, //sms like: set low battery on/off
    GET_LOW_BATTERY, //sms: get low battery
    SET_ALARM, //sms like: set alarm on/off
    GET_ALARM, //sms: get alarm
    SET_ALARM_SENSITY, //sms like: set alarm sensity 0-32767
    GET_ALARM_SENSITY, //sms: get alarm sensity
    SET_MODE, //sms like: set mode def/sleep
    GET_MODE, //sms: get mode
    INVALID
};

struct DateTime
{
    uint16_t year{0};
    uint8_t month{0}; //0-11
    uint8_t day{0}; //0-30
    uint8_t hours{0}; //0-23
    uint8_t minutes{0}; //0-59
    uint8_t seconds{0}; //0-59
};

enum class LOW_BATTERY_STATE
{
    ON = 0,
    OFF
};

enum class ALARM_STATE
{
    OFF = 0,
    ON
};

struct DefaultCommandData
{
    DEFAULT_COMMANDS cmd;
    Phone phone;
    DateTime date_time;

    union
    {
        LOW_BATTERY_STATE low_battery_state;
        ALARM_STATE alarm_state;
        int sensity;
        WORK_MODE work_mode;
    };

    explicit DefaultCommandData(const Phone& phone)
        : cmd(DEFAULT_COMMANDS::INVALID),
        phone(phone)
    {}

    explicit DefaultCommandData(DEFAULT_COMMANDS command, const Phone& phone)
        : cmd(command),
        phone(phone)
    {}

    explicit DefaultCommandData(DEFAULT_COMMANDS command, const Phone& phone, LOW_BATTERY_STATE state)
        : cmd(command),
        phone(phone),
        low_battery_state(state)
    {}

    explicit DefaultCommandData(DEFAULT_COMMANDS command, const Phone& phone, ALARM_STATE state)
        : cmd(command),
        phone(phone),
        alarm_state(state)
    {}

    explicit DefaultCommandData(DEFAULT_COMMANDS command, const Phone& phone, int sensity)
        : cmd(command),
        phone(phone),
        sensity(sensity)
    {}

    explicit DefaultCommandData(DEFAULT_COMMANDS command, const Phone& phone, WORK_MODE mode)
        : cmd(command),
        phone(phone),
        work_mode(mode)
    {}

    explicit DefaultCommandData(DEFAULT_COMMANDS command, const Phone& phone, const DateTime& date)
        : cmd(command),
        phone(phone),
        date_time(date)
    {}

};