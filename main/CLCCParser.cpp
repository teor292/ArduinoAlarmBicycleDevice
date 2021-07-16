#include "CLCCParser.h"

CLCC_CALL_STATE CLCCParser::Parse(SafeString& str)
{
    const static char CLCC[] = "+CLCC:";
    if (!str.startsWith(CLCC)) return CLCC_CALL_STATE::ERROR_PARSE;

    int index = str.indexOf(',', sizeof(CLCC));
    if (-1 == index) return CLCC_CALL_STATE::ERROR_PARSE;

    index = str.indexOf(',', index + 1);
    if (-1 == index) return CLCC_CALL_STATE::ERROR_PARSE;

    int last_index = str.indexOf(',', index + 1);
    if (-1 == last_index) return CLCC_CALL_STATE::ERROR_PARSE;

    str.substring(str, index + 1, last_index);

    int value = 0;
    if (!str.toInt(value)) return CLCC_CALL_STATE::ERROR_PARSE;

    switch (value)
    {
    case 0: return CLCC_CALL_STATE::ACTIVE;
    case 1: return CLCC_CALL_STATE::HELD;
    case 2: return CLCC_CALL_STATE::DIALING;
    case 3: return CLCC_CALL_STATE::ALERTING;
    case 4: return CLCC_CALL_STATE::INCOMING;
    case 5: return CLCC_CALL_STATE::WAITING;
    case 6: return CLCC_CALL_STATE::DISCONNECT;
    }
    return CLCC_CALL_STATE::UNKNOWN;
}