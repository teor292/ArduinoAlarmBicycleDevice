#pragma once

#include "header.h"
#include <SafeString.h>


class SmsBuffer
{
    public:

        explicit SmsBuffer();

        //true if consume sms symbol
        bool Write(char c);
        //if buffer is filled or valid sms getted
        bool IsFilled() const;

        SafeString& GetSms();

        void Clear();
    protected:

        bool start_readed_{false};
        char buffer_[SMS_BUF];
        SafeString buffer_str_;
        uint8_t cnt_ln_{0};
};