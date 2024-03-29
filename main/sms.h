#pragma once

#include "SafeString.h"

#include <Stream.h>
#include "BlockTimeReader.h"

#include "header.h"
#include "Phone.h"


class Sms
{
    public:

        explicit Sms(Stream &serial, BlockTimeReader& reader, SafeString& g_string,  WaitCallback callback = nullptr);

        void DeleteAllSms(SafeString& buffer);

        void SendSms(const char *text);

        void SetPhone(const char* phone);

    private:

        Stream &serial_;
        BlockTimeReader& reader_;
        SafeString& g_string_;
        WaitCallback callback_;

        Phone phone_;

        bool send_sms_one_(const char *text);

        void wait_for_(int time_ms);
};
