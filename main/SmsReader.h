#pragma once

#include "DefaultCommands.h"
#include "GPSCommands.h"
#include <ArxSmartPtr.h>
#include "header.h"
#include "SmsBuffer.h"

enum class SMS_DATA_TYPE
{
    NONE = 0,
    DEFAULT_CMD = 1,
    #if defined(GPS)
    GPS_CMD = 2
    #endif
};

struct SmsData
{
    explicit SmsData() = default;
    explicit SmsData(const DefaultCommandData& cmd_data)
        : type{SMS_DATA_TYPE::DEFAULT_CMD},
          cmd{cmd_data}
        {
        }

    #if defined(GPS)
    explicit SmsData(const GPSCommandData& cmd_data)
        : type{SMS_DATA_TYPE::GPS_CMD},
        cmd{cmd_data}
        {
        }
    #endif

    SMS_DATA_TYPE type{SMS_DATA_TYPE::NONE};
    union Commands
    {
        Commands()
        {
            memset(&default_command, 0, sizeof(*this));
        }
        Commands(const DefaultCommandData& cmd_data)
            : default_command{cmd_data}
        {
        }
        DefaultCommandData default_command;
        #if defined(GPS)

        Commands(const GPSCommandData& cmd_data)
            : gps_command{cmd_data}
        {
        }
        GPSCommandData gps_command;
        #endif
    } cmd;

    bool IsEmpty() const
    {
        return SMS_DATA_TYPE::NONE == type;
    }
};


class SmsReader
{
    public:

        bool Write(char c);

        bool IsFilled() const;

        SmsData Work();


    protected:

        SmsBuffer buffer_;

        static bool cmt_extract_phone_number_(SafeString& source, SafeString& dst);
        static bool extract_date_(SafeString& source, SafeString& dst, unsigned char count_comas_omit);
        static bool extract_date_(SafeString& source, DateTime& dst, unsigned char count_comas_omit);
        static SmsData parse_command_(SafeString& sms_string, DateTime& date, Phone& phone);
};