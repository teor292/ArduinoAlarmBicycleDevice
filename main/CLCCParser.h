#pragma once


#include <SafeString.h>

enum class CLCC_CALL_STATE
{
    ERROR_PARSE, 
    DIALING, //start dial phone
    ALERTING, //connection started
    ACTIVE, //accept call
    HELD,
    INCOMING,
    WAITING,
    DISCONNECT,
    UNKNOWN = 0xFF
};

class CLCCParser
{
    public:

        static CLCC_CALL_STATE Parse(SafeString& str);

};