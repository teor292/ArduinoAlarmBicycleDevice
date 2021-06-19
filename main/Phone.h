#pragma once

#include "header.h"
#include <string.h>
#include <SafeString.h>

struct Phone
{
    char phone[PHONE_BUF_LENGTH]{};

    Phone& operator = (const Phone& data)
    {
        for (int i = 0; i <  PHONE_BUF_LENGTH; ++i)
        {
            phone[i] = data.phone[i];
        }
    }

    bool operator == (const char* pp) const
    {
        if (nullptr == pp) return false;
        int len = 0;
        int diff = 0;
        if ('\"' != pp[0])
        {
            diff = 1;
            len = strnlen(pp, PHONE_BUF_LENGTH - 3);
        } else
        {
            len = strnlen(pp, PHONE_BUF_LENGTH - 1);
        }
        for (int i = 0; i < len; ++i)
        {
            if (phone[i + diff] != pp[i]) return false;
        }
        return true;    
    }

    bool operator != (const char* data) const
    {
        return !((*this) == data);
    }

    bool operator == (const Phone& data) const
    {
        return 0 == strncmp(phone, data.phone, PHONE_BUF_LENGTH);
    }

    bool operator != (const Phone& data) const
    {
        return !((*this) == data);
    }

    Phone& operator = (const char *data)
    {
        createSafeStringFromCharArray(tmp, phone);
        tmp = data;
        return *this;
    }

    bool AssignData(const char *data, size_t length)
    {
        if (PHONE_BUF_LENGTH - 3 < length) return false;
        phone[0] = '\"';
        for (int i = 0; i < length; ++i)
        {
            phone[i + 1] = data[i];
        }
        phone[length + 1] = '\"';
        phone[length + 2] = 0;
        return true;
    }
};