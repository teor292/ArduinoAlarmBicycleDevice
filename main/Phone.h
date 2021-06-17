#pragma once

#include "header.h"
#include <cstring>
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
};