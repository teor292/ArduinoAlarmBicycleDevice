#include "BlockTimeReader.h"

BlockTimeReader::BlockTimeReader(SoftwareSerial& serial, millisDelay &delay_object)
 : serial_(serial),
 time_delay_(delay_object)
 {}

bool BlockTimeReader::ReadChar(char& c, const int timeout)
{
    time_delay_.start(timeout);
    while (!time_delay_.justFinished())
    {
      if (!serial_.available()) continue;
      c = (char)serial_.read();
      return true;
    } 
    return false;
}

bool BlockTimeReader::read_response_(SafeString& result, const int timeout, unsigned char count_new_lines)
{
    //must ignore input and get count end of line after input
    result.clear();
    time_delay_.start(timeout);

    while (!time_delay_.justFinished())
    {
        if (!serial_.available()) continue;
        char c = (char)serial_.read();
        if (10 == c)
        {
            break;
        }
    }
    unsigned char cnt = 0;
    while (cnt < count_new_lines 
        && !time_delay_.justFinished() 
        && time_delay_.isRunning())
    {
        if (!serial_.available()) continue;
        char c = (char)serial_.read();
        result += c;
        if (10 == c)
        {
            ++cnt;
        }
    }
    PRINT(F("Current line: "));
    PRINTLN(result);
    PRINTLN(F("Current line end"));

    return cnt == count_new_lines;
}

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

bool BlockTimeReader::ReadSomeResponse(SafeString& result, const int timeout)
{
    return read_response_(result, timeout, 3);
}

bool BlockTimeReader::ReadLine(SafeString& test_string, const int timeout)
{
    test_string.clear();
    return NClReadLine(test_string, timeout);
}

bool BlockTimeReader::NClReadLine(SafeString& test_string, const int timeout)
{
    time_delay_.start(timeout);
    while (!time_delay_.justFinished())
    {
        if (!serial_.available()) continue;
        char c = (char)serial_.read();
        test_string += c;
        if (10 == c) return true;
    }
    PRINT(F("Current line: "));
    PRINTLN(test_string);

    return false;
}

bool BlockTimeReader::ReadUntil(SafeString& buffer, const int timeout, const char *what)
{
    buffer.clear();
    time_delay_.start(timeout);

    while (!time_delay_.justFinished())
    {
        if (!serial_.available()) continue;
        char c = (char)serial_.read();
        buffer += c;
        if (10 != c) continue;

        buffer.trim();
        if (buffer == what) return true;
        PRINTLN(buffer);
        buffer.clear();
    }
    return false;
}