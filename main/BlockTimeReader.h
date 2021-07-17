#pragma once

#include "MillisCallback.h"
#include "SafeString.h"
#include "header.h"


class BlockTimeReader
{
    public:

        explicit BlockTimeReader(MillisReadDelay& millis);

        bool ReadStatusResponse(SafeString& result, const int timeout, 
            const char *ok = nullptr, const char* error = nullptr);
        bool ReadLine(SafeString& test_string, const int timeout);
        bool NClReadLine(SafeString& test_string, const int timeout);
        bool ReadUntil(SafeString& buffer, const int timeout, const char *what);
        bool ReadUntil(SafeString& buffer, const int timeout, char what);

    private:

        MillisReadDelay& millis_;

        bool read_until_(SafeString& buffer, const int timeout, const char *what, char c);
        bool nc_read_until_(SafeString& buffer, const int timeout, const char *what, char c);

};