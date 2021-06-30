#pragma once

#include <stdint.h>


class AbstractNextAwakeTimeGetter
{
    public:
        virtual ~AbstractNextAwakeTimeGetter(){}
        //return diff from current time
        virtual uint32_t NextNeccessaryDiffTime(uint32_t current_time) = 0;
};