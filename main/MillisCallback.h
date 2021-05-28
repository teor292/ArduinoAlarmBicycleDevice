#pragma once


#include <millisDelay.h>
#include <Stream.h>

using WaitCallback = void(*)();

class MillisReadDelay : public millisDelay
{
    public:

        explicit MillisReadDelay(Stream& read_stream, WaitCallback callback = nullptr)
            : read_stream_(read_stream),
             callback_(callback)
        {}

        bool Read(int& c_out)
        {
            while (!justFinished())
            {
                if (!read_stream_.available()) 
                {
                    if (nullptr != callback_)
                    {
                        callback_();
                    }
                    continue;
                }
                c_out = read_stream_.read();
                return true;
            }
            return false;
        }

    protected:

        Stream& read_stream_;
        WaitCallback callback_;
};