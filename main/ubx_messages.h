#pragma once

#include <stdint.h>
#include <Stream.h>
#include "MillisCallback.h"

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

    //ack message is also nack message. So for correct check
    //it is neccessary to override this func in ACK message by ACK type
    bool CheckMessageType(const UBX_Base& msg) const
    {
        return clss == msg.clss && id == msg.id;
    }
};

enum class READ_UBX_RESULT : uint8_t
{
    OK = 0,
    ERROR_TIMEOUT = 1,
    ERROR_MESSAGE_BY_LENGTH,
    ERROR_MESSAGE_BY_TYPE, 
    ERROR_CRC
};

class AbstractNonUBXCallable
{
    public:

        virtual ~AbstractNonUBXCallable(){}
        virtual void NonUBXSymbol(uint8_t c) = 0;
};  

using NonUbxCallback = AbstractNonUBXCallable*;

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

        READ_UBX_RESULT Read(Stream& stream, NonUbxCallback callback = nullptr, int timeout = 1000,
             WaitCallback wait_callback = nullptr)
        {   
            MillisReadDelay millis(stream, wait_callback);
            millis.start(timeout);
            auto header_result = read_header_(callback, millis);
            if (READ_UBX_RESULT::OK != header_result) return header_result;

            auto body_header_result = read_header_body_part_(millis);
            if (READ_UBX_RESULT::OK != body_header_result) return body_header_result;

            if (sizeof(message) - sizeof(UBX_Base) != message.length)
            {
                empty_read_(message.length, millis);
                return READ_UBX_RESULT::ERROR_MESSAGE_BY_LENGTH;
            }
            const T test_msg;
            if (!test_msg.CheckMessageType(message))
            {
                empty_read_(message.length, millis);
                return READ_UBX_RESULT::ERROR_MESSAGE_BY_TYPE;
            }

            auto ptr = reinterpret_cast<uint8_t*>(&message) + sizeof(UBX_Base);
            auto remain_read = message.length + 2 * sizeof(uint8_t);
            auto message_result = read_to_buffer_(ptr, remain_read, millis);

            if (READ_UBX_RESULT::OK != message_result) return message_result;
            
            auto read_ck_a = ck_a_;
            auto read_ck_b = ck_b_;
            crc_();
            if (read_ck_a != ck_a_
                || read_ck_b != ck_b_) return READ_UBX_RESULT::ERROR_CRC;

            return READ_UBX_RESULT::OK;
        }

    private:
        uint8_t ck_a_{0};
        uint8_t ck_b_{0};

        void crc_()
        {
            ck_a_ = 0;
            ck_b_ = 0;
            auto ptr = reinterpret_cast<uint8_t*>(&message); 
            for (size_t i = 0; i < sizeof(message); ++i)
            {
                ck_a_ = ck_a_ + ptr[i];
                ck_b_ = ck_b_ + ck_a_;
            }
        }

        READ_UBX_RESULT read_header_(NonUbxCallback callback, MillisReadDelay &millis)
        {
            return read_same_buffer_(header_, sizeof(header_), callback, millis);
        }
        READ_UBX_RESULT read_header_body_part_(MillisReadDelay &millis)
        {
            return read_to_buffer_(reinterpret_cast<uint8_t*>(&message),
                sizeof(UBX_Base),
                millis);
        }            
        
        static READ_UBX_RESULT read_to_buffer_(uint8_t* buf,
            size_t size_to_write,
            MillisReadDelay &millis)
        {
            int c_out = 0;
            while (size_to_write > 0
                && millis.Read(c_out))
            {
                *buf = static_cast<uint8_t>(c_out);
                ++buf;
                --size_to_write;
            }
            return 0 == size_to_write ? READ_UBX_RESULT::OK : READ_UBX_RESULT::ERROR_TIMEOUT;
        }

        static void empty_read_(size_t size_to_read, MillisReadDelay &millis)
        {
            int c_out = 0;
            while (size_to_read > 0
                && millis.Read(c_out))
            {
                --size_to_read;
            }
        }

        static READ_UBX_RESULT read_same_buffer_(const uint8_t* buf, 
            const size_t total_size, 
            NonUbxCallback callback, 
            MillisReadDelay &millis)
        {
            size_t current_pos = 0;
            int c_out = 0;
            while (current_pos < total_size
                && millis.Read(c_out))
            {
                uint8_t c = static_cast<uint8_t>(c_out);
                if (c == buf[current_pos])
                {
                    ++current_pos;
                }
                else
                {
                    send_to_callback_(buf, current_pos, callback);
                    current_pos = 0;
                    send_to_callback_(&c, 1, callback);
                }
            }
            if (total_size == current_pos) return READ_UBX_RESULT::OK;
            return READ_UBX_RESULT::ERROR_TIMEOUT;
        }

        static void send_to_callback_(const uint8_t *buf, size_t size, NonUbxCallback callback)
        {
            if (nullptr != callback)
            {
                for (size_t i = 0; i < size; ++i)
                {
                    callback->NonUBXSymbol(buf[i]);
                }
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
    TIME_REF timeRef{TIME_REF::GPS_TIME};
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
            //go into backmode if true (what is back mode?)
            uint8_t backup : 1;
        };
    };
};

//output message for check aknowledge of send message
struct UBX_ACK_BASE_ : UBX_Base
{
    UBX_ACK_BASE_()
        : UBX_Base{0x5, 0x0, 0x0}
    {}
    uint8_t clsID{0};
    uint8_t msgID{0};

    bool CheckMessageType(const UBX_ACK_BASE_& msg) const
    {
        //0x0 == NACK, 0x1 == ACK
        return clss == msg.clss && (id == 0x0 || id == 0x1);
    }
};

union UBX_CFG_HELPER
{
    uint32_t mask{0};
    struct
    {
        uint32_t ioPort : 1;
        uint32_t msgConf: 1;
        uint32_t infMsg : 1;
        uint32_t navConf : 1;
        uint32_t rxmConf : 1;
        uint32_t : 3;
        uint32_t senConf : 1;
        uint32_t rinvConf : 1;
        uint32_t antConf : 1;
        uint32_t logConf : 1;
        uint32_t ftsConf : 1;
    };
};

//message for resettings, saving and loading config
struct UBX_CFG_CFG_BASE_ : UBX_Base
{
    UBX_CFG_CFG_BASE_()
        : UBX_Base{0x06, 0x09, 12}
    {}

    UBX_CFG_HELPER clearMask;
    UBX_CFG_HELPER saveMask;
    UBX_CFG_HELPER loadMask;
    
};

enum class RESET_GPS_MODE : uint8_t
{
    HARDWARE = 0x0,
    SOFTWARE = 0x1,
    SOFTWARE_GNSS = 0x2,
    HARDWARE_AFTER_SHUTDOWN = 0x4,
    CONTROLLED_GNSS_STOP = 0x8,
    CONTROLLED_GNSS_START = 0x9
};

struct UBX_CFG_RST_BASE_ : UBX_Base
{
    UBX_CFG_RST_BASE_()
        : UBX_Base{0x06, 0x04, 4}
    {}
    union
    {
        uint16_t navBbrMask{0};
        struct
        {
            uint16_t eph : 1;
            uint16_t alm : 1;
            uint16_t health : 1;
            uint16_t klob : 1;
            uint16_t pos : 1;
            uint16_t clkd : 1;
            uint16_t osc : 1;
            uint16_t utc : 1;
            uint16_t rtc : 1;
            uint16_t : 2;
            uint16_t sfdr : 1;
            uint16_t vmon : 1;
            uint16_t tct : 1;
            uint16_t : 1;
            uint16_t aop : 1;
        };
    };

    RESET_GPS_MODE resetMode{RESET_GPS_MODE::HARDWARE};
    uint8_t reserved1{0};

}; 

using UBX_CFG_PM2 = UBX_MESSAGE_<UBX_CFG_PM2_BASE_>;
using UBX_CFG_RXM = UBX_MESSAGE_<UBX_CFG_RXM_BASE_>;
using UBX_CFG_REQUEST_RATE = UBX_MESSAGE_<UBX_CFG_REQUEST_RATE_BASE_>;
using UBX_CFG_RATE = UBX_MESSAGE_<UBX_CFG_RATE_BASE_>;
using UBX_RXM_PMREQ = UBX_MESSAGE_<UBX_RXM_PMREQ_BASE_>;
using UBX_ACK = UBX_MESSAGE_<UBX_ACK_BASE_>;
using UBX_CFG_CFG = UBX_MESSAGE_<UBX_CFG_CFG_BASE_>;
using UBX_CFG_RST = UBX_MESSAGE_<UBX_CFG_RST_BASE_>;