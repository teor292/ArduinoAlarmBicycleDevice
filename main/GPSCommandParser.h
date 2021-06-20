#pragma once

#include "gps_header.h"

#if defined(GPS)

#include "GPSCommands.h"
#include <SafeString.h>
#include "Phone.h"



class GPSCommandParser
{
    public:

        static GPSCommandData ParseSms(SafeString& sms_string, Phone& source_phone);
    
    private:

        static GPSCommandData parse_get_(SafeString& sms_string, Phone& source_phone);
        static GPSCommandData parse_set_(SafeString& sms_string, Phone& source_phone);
        static GPSCommandData parse_gps_(SafeString& sms_string, Phone& source_phone);

        static GPSCommandData parse_get_send_(SafeString& sms_string, Phone& source_phone);
        static GPSCommandData parse_set_fix_(SafeString& sms_string, Phone& source_phone);
        static GPSCommandData parse_set_vibro_(SafeString& sms_string, Phone& source_phone);
        static GPSCommandData parse_set_send_(SafeString& sms_string, Phone& source_phone);
        static GPSCommandData parse_set_send_remove_(SafeString& sms_string, Phone& source_phone);
        //static GPSCommandData parse_set_send_alarm_(SafeString& sms_string, Phone& source_phone);
        static bool parse_time_(SafeString& sms_string, uint32_t& result);

};


#endif