#pragma once

#include <stdint.h>
#include <Stream.h>

struct UBX_Base
{
    UBX_Base(uint8_t clss, uint8_t id, uint16_t length = 0)
        : clss{clss},
        id{id},
        length{length}
    {}
    uint8_t clss;
    uint8_t id;
    uint16_t length;
};


template<typename T>
class UBX_MESSAGE_
{
        uint8_t header_[2] = { 0xb5, 0x62};
    public:

        T message;

        void Send(Stream& stream)
        {     
            crc_();
            auto ptr = reinterpret_cast<uint8_t*>(this);
            for (size_t i = 0; i < sizeof(*this); ++i)
            {
                stream.write(ptr[i]);
            }
        }
    private:
        uint8_t ck_a_{0};
        uint8_t ck_b_{0};

        void crc_()
        {
            auto ptr = reinterpret_cast<uint8_t*>(&message); 
            for (size_t i = 0; i < sizeof(message); ++i)
            {
                ck_a_ = ck_a_ + ptr[i];
                ck_b_ = ck_b_ + ck_a_;
            }
        }
};

enum class POWER_SAVING_MODES : uint8_t
{
    PSMOO = 0x0,
    PSMCT = 0x1
};

struct UBX_CFG_PM2_BASE_ : UBX_Base
{
    UBX_CFG_PM2_BASE_() 
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
    };
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

    static UBX_CFG_PM2_BASE_ GetDefaultPCMT()
    {
        UBX_CFG_PM2_BASE_ result;
        result.updatePeriod = 5000;
        result.searchPeriod = static_cast<uint32_t>(300) * 1000; //300 seconds
        result.mode = static_cast<uint32_t>(POWER_SAVING_MODES::PSMCT);
        return result;
    };

    static UBX_CFG_PM2_BASE_ GetDefaultPCMOO()
    {
         UBX_CFG_PM2_BASE_ result;
         result.updatePeriod = static_cast<uint32_t>(60) * 1000;
         result.searchPeriod = static_cast<uint32_t>(300) * 1000;
         result.mode = static_cast<uint32_t>(POWER_SAVING_MODES::PSMOO);
         return result;
    }
};

enum class LP_MODE : uint8_t
{
    CONTINOUS = 0,
    POWER_SAVING = 1
};

struct UBX_CFG_RXM_BASE_ : UBX_Base
{
    UBX_CFG_RXM_BASE_()
        : UBX_Base{0x06, 0x11, 2}
    {}
    uint8_t reserved1{0};
    LP_MODE lpMode{LP_MODE::CONTINOUS};
};

struct UBX_CFG_REQUEST_RATE_BASE_ : UBX_Base
{
    UBX_CFG_REQUEST_RATE_BASE_()
        : UBX_Base{0x06, 0x08, 0}
    {}
};

enum class TIME_REF : uint16_t
{
    UTC_TIME = 0,
    GPS_TIME = 1,
    GLONASS_TIME = 2,
    BeiDou_TIME = 3,
    Galileo_TIME = 4
};

struct UBX_CFG_RATE_BASE_ : UBX_Base
{
    UBX_CFG_RATE_BASE_()
        : UBX_Base{0x06, 0x08, 6}
    {}

    uint16_t measRate{1000};
    uint16_t navRate{1};
    TIME_REF timeRef{TIME_REF::UTC_TIME};
};

struct UBX_RXM_PMREQ_BASE_ : UBX_Base
{
    UBX_RXM_PMREQ_BASE_()
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
};

using UBX_CFG_PM2 = UBX_MESSAGE_<UBX_CFG_PM2_BASE_>;
using UBX_CFG_RXM = UBX_MESSAGE_<UBX_CFG_RXM_BASE_>;
using UBX_CFG_REQUEST_RATE = UBX_MESSAGE_<UBX_CFG_REQUEST_RATE_BASE_>;
using UBX_CFG_RATE = UBX_MESSAGE_<UBX_CFG_RATE_BASE_>;
using UBX_RXM_PMREQ = UBX_MESSAGE_<UBX_RXM_PMREQ_BASE_>;