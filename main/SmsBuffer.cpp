#include "SmsBuffer.h"

namespace
{
    const char CMT[] = "CMT";
}

SmsBuffer::SmsBuffer()
    :buffer_str_(sizeof(buffer_),buffer_, buffer_, "str", true)
{}

bool SmsBuffer::Write(char c)
{
    if (!start_readed_ && '+' != c) return false;
    if (start_readed_)
    {
        //check if sms
        if (buffer_str_.length() < sizeof(CMT) - 1)
        {
            buffer_str_ += c;
            if (buffer_str_[buffer_str_.length() - 1] != CMT[buffer_str_.length() - 1])
            {
                Clear();
                return false;
            }
            //return false because don't know CMT it is or not
            return false;
        }
        //already filled
        if (IsFilled()) return false;
        //sms too big, ignore symbol
        if (buffer_str_.length() == SMS_BUF - 2) 
        {
            buffer_str_ += (char)10;
            ++cnt_ln_;
            if (!IsFilled())
            {
                Clear();
                return false;
            }
            return 10 == c;
        }
        //must never occured
        //if (buffer_str_.length() > SMS_BUF - 2) return false;

        buffer_str_ += c;
        if (10 == c)
        {
            ++cnt_ln_;
        }

    } else
    {
        start_readed_ = true;
    }
    return true;
}


bool SmsBuffer::IsFilled() const
{
    //two new lines in sms, multiline sms not supported
    return 2 == cnt_ln_;
}

 SafeString& SmsBuffer::GetSms()
 {
     return buffer_str_;
 }

void SmsBuffer::Clear()
{
    buffer_str_.clear();
    start_readed_ = false;
    cnt_ln_ = 0;
}