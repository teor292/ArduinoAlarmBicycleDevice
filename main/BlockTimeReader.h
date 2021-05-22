#pragma once

#include <millisDelay.h>
#include <Stream.h>
#include "SafeString.h"
#include "header.h"


class BlockTimeReader
{
    public:

        explicit BlockTimeReader(Stream& serial, millisDelay &delay_object);

        bool ReadStatusResponse(SafeString& result, const int timeout);
        bool ReadLine(SafeString& test_string, const int timeout);
        bool NClReadLine(SafeString& test_string, const int timeout);
        bool ReadUntil(SafeString& buffer, const int timeout, const char *what);
        bool ReadUntil(SafeString& buffer, const int timeout, char what);

    private:

        Stream& serial_;
        millisDelay& time_delay_;

        bool read_until_(SafeString& buffer, const int timeout, const char *what, char c);
        bool nc_read_until_(SafeString& buffer, const int timeout, const char *what, char c);

};