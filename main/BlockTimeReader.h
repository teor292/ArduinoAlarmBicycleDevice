#pragma once

#include <millisDelay.h>
#include <SoftwareSerial.h>
#include "SafeString.h"
#include "header.h"


class BlockTimeReader
{
    public:

        explicit BlockTimeReader(SoftwareSerial& serial, millisDelay &delay_object);

        bool ReadChar(char& c, const int timeout);
        bool ReadStatusResponse(SafeString& result, const int timeout);
        bool ReadLine(SafeString& test_string, const int timeout);
        bool NClReadLine(SafeString& test_string, const int timeout);
        bool ReadUntil(SafeString& buffer, const int timeout, const char *what);

    private:

        SoftwareSerial& serial_;
        millisDelay& time_delay_;

        bool read_response_(SafeString& result, const int timeout, unsigned char count_new_lines);

};