#pragma once

enum class GPS_ERROR_CODES
{
    OK = 0,
    UBX_READ_TIMEOUT = 1,
    UBX_ERROR_LENGTH,
    UBX_ERROR_TYPE, 
    UBX_ERROR_CRC,
    UBX_NACK_MESSAGE,
    UBX_OTHER_MESSAGE,
    NOT_SUPPORTED,
    NOT_INITIALIZED
};

#define GPS_OK(x) (GPS_ERROR_CODES::OK == (x))