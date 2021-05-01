#pragma once

#include "SafeString.h"

#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include "BlockTimeReader.h"

#include "header.h"





class Sms
{
    public:

        explicit Sms(SoftwareSerial &serial, BlockTimeReader& reader, SafeString& g_string);
#if defined(CMT)
        bool TryReadForwardSmsFromSerial(SafeString& result);
        void DeleteAllSms(SafeString& buffer);
#endif
#if defined(CMGR)
        bool ReadFromSerial(SafeString& cmti);
        void DeleteReadIncomeMessages();
#endif

        const char* GetPhone() const;
        const char* GetTime() const;
        char* GetText();

        void SendSms(const char *text);

        void SetPhone(const char* phone);

    private:

        SoftwareSerial &serial_;
        BlockTimeReader& reader_;
        SafeString& g_string_;

        char phone_[19];
        char date_[18];
        char text_[25];

        bool extract_date_(SafeString& source, SafeString& dst, unsigned char count_comas_omit);

#if defined(CMGR)
        static bool extract_phone_number_(SafeString& source, SafeString& dst);
#endif

#if defined(CMT)
        static bool cmt_extract_phone_number_(SafeString& source, SafeString& dst);
#endif
};
