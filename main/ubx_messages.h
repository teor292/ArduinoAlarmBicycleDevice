#pragma once

#include <cstdint>

struct UBX_Base
{
    UBX_Base(uint8_t clss, uint8_t id, uint16_t length = 0)
        : clss{clss},
        id{id},
        length{length}
    {}
    uint8_t header[2] = { 0xb5, 0x62};
    uint8_t clss;
    uint8_t id;
    uint16_t length;
};

struct UBX_Crc
{
    uint8_t ck_a{0};
    uint8_t ck_b{0};

    void Calculate(uint8_t* ptr, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            ck_a = ck_a + ptr[i];
            ck_b = ck_b + ck_a;
        }
    }
};

enum class POWER_SAVING_MODES : uint8_t
{
    PSMOO = 0x0,
    PSMCT = 0x1
};

struct UBX_CFG_PM2 : UBX_Base
{
    UBX_CFG_PM2() 
        : UBX_Base{0x06, 0x3b, 44}
    {}

    uint8_t version{0x01};
    uint8_t reserved1{0};
    //4 minutes max by default
    uint8_t maxStartupStateDur{240};
    uint8_t reserved2{0};
    union
    {
        uint32_t flags{0};
        struct
        {
            uint32_t :4;
            uint32_t extintSel : 1;
            uint32_t extintWake : 1;
            uint32_t extintBackup : 1;
            uint32_t : 1;
            uint32_t limitPeakCurr : 2;
            uint32_t waitTimeFix : 1;
            uint32_t updateRTC : 1;
            uint32_t updateEPH : 1;
            uint32_t : 3;
            uint32_t doNotEnterOff : 1;
            uint32_t mode : 2;    
        };
    }
    //if 0 -> receiver will never retry a fix and it will wait for external events
    uint32_t updatePeriod{0};
    //If set to 0, the receiver will never retry a startup
    uint32_t searchPeriod{0};
    uint32_t gridOffset{0};
    //time in Tracking state by default 1 min
    uint16_t onTime{60};
    //min time by default 1 min
    uint16_t minAcqTime{60};
    uint8_t reserved3[20]{};
    UBX_Crc crc;

    static UBX_CFG_PM2 GetDefaultPCMT()
    {
        UBX_CFG_PM2 result;
        result.updatePeriod = 5000;
        result.searchPeriod = 300 * 1000; //300 seconds
        result.mode = POWER_SAVING_MODES::PSMCT;
        return result;
    };

    static UBX_CFG_PM2 GetDefaultPCMOO()
    {
         UBX_CFG_PM2 result;
         result.updatePeriod = 60 * 1000;
         result.searchPeriod = 300 * 1000;
         result.mode = POWER_SAVING_MODES::PSMOO;
         return result;
    }
};

enum class LP_MODE : uint8_t
{
    CONTINOUS = 0,
    POWER_SAVING = 1
}

struct UBX_CFG_RXM : UBX_Base
{
    UBX_CFG_RXM()
        : UBX_Base{0x06, 0x11, 2}
    {}
    uint8_t reserved1{0};
    LP_MODE lpMode{LP_MODE::CONTINOUS};
    UBX_Crc crc;
};

struct UBX_CFG_REQUEST_RATE : UBX_Base
{
    UBX_CFG_RATE_Request()
        : UBX_Base{0x06, 0x08, 0}
    UBX_Crc crc;
};

enum class TIME_REF : uint16_t
{
    UTC_TIME = 0,
    GPS_TIME = 1,
    GLONASS_TIME = 2;
    BeiDou_TIME = 3,
    Galileo_TIME = 4
};

struct UBX_CFG_RATE : UBX_Base
{
    UBX_CFG_RATE()
        : UBX_Base{0x06, 0x08, 6}
    {}

    uint16_t measRate{1000};
    uint16_t navRate{1};
    TIME_REF timeRef{TIME_REF::UTC_TIME};
    UBX_Crc crc;
};

struct UBX_RXM_PMREQ : UBX_Base
{
    UBX_RXM_PMREQ()
        : UBX_Base{0x02, 0x41, 8}
    {}
    //max 12 days, default 0 - not awake
    uint32_t duration{0};
    union
    {
        uint32_t flags{0};
        struct
        {
            uint8_t : 1;
            //go into backmode if true
            uint8_t backup : 1;
        };
    };
    UBX_Crc crc;
};