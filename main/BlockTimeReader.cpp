#include "BlockTimeReader.h"

BlockTimeReader::BlockTimeReader(Stream& serial, millisDelay &delay_object)
 : serial_(serial),
 time_delay_(delay_object)
 {}


bool BlockTimeReader::ReadStatusResponse(SafeString& result, const int timeout)
{
    //must ignore input and get count end of line after input
    result.clear();
    time_delay_.start(timeout);

    while (!time_delay_.justFinished())
    {
        if (!serial_.available()) continue;
        char c = (char)serial_.read();
        result += c;
        if (10 == c) //LF
        {
            auto index1 = result.indexOf("OK");
            auto index2 = result.indexOf("ERROR");
            if (-1 != index1 
                || -1 != index2)
            {
                PRINT(F("Current line: "));
                PRINTLN(result);
                PRINTLN(F("Current line end"));
                return true;
            }
        }
    }
    PRINT(F("Current line: "));
    PRINTLN(result);
    PRINTLN(F("Current line end"));
    return false;

}

bool BlockTimeReader::ReadLine(SafeString& test_string, const int timeout)
{
    test_string.clear();
    return nc_read_until_(test_string, timeout, nullptr, 10);
}

bool BlockTimeReader::NClReadLine(SafeString& test_string, const int timeout)
{
    return nc_read_until_(test_string, timeout, nullptr, 10);
}

bool BlockTimeReader::ReadUntil(SafeString& buffer, const int timeout, const char *what)
{
    return read_until_(buffer, timeout, what, 10);
}

bool BlockTimeReader::ReadUntil(SafeString& buffer, const int timeout, char what)
{
    return read_until_(buffer, timeout, nullptr, what);
}


bool BlockTimeReader::read_until_(SafeString& buffer, const int timeout, const char *what, char c)
{
    buffer.clear();
    return nc_read_until_(buffer, timeout, what, c);
}

bool BlockTimeReader::nc_read_until_(SafeString& buffer, const int timeout, const char *what, char c)
{
    time_delay_.start(timeout);

    while (!time_delay_.justFinished())
    {
        if (!serial_.available()) continue;
        char c1 = (char)serial_.read();
        buffer += c1;
        if (c != c1) continue;

        if (nullptr == what) return true;
        
        buffer.trim();
        if (buffer.startsWith(what)) 
        {
            PRINT("nc_read_until true: ");
            PRINTLN(buffer);
            return true;
        }
        PRINT("nc_read_until false: ");
        PRINTLN(buffer);
        buffer.clear();
    }
    return false;
}