#pragma once

#include "SafeString.h"

#include <Stream.h>
#include "BlockTimeReader.h"

#include "header.h"
#include "Phone.h"


class Sms
{
    public:

        explicit Sms(Stream &serial, BlockTimeReader& reader, SafeString& g_string);

        bool TryReadForwardSmsFromSerial(SafeString& result);
        void DeleteAllSms(SafeString& buffer);

        const char* GetPhone() const;
        const char* GetTime() const;
        char* GetText();

        void SendSms(const char *text);

        void SetPhone(const char* phone);

    private:

        Stream &serial_;
        BlockTimeReader& reader_;
        SafeString& g_string_;

        Phone phone_;
        char date_[18];
        char text_[25];

        bool extract_date_(SafeString& source, SafeString& dst, unsigned char count_comas_omit);

        static bool cmt_extract_phone_number_(SafeString& source, SafeString& dst);
};
