#pragma once

#include "DefaultCommands.h"
#include <SafeString.h>

class DefaultCommandParser
{
    public:

        static DefaultCommandData ParseSms(SafeString& sms_string, Phone& source_phone);
    
    private:

        static DefaultCommandData parse_get_(SafeString& sms_string, Phone& source_phone);
        static DefaultCommandData parse_set_(SafeString& sms_string, Phone& source_phone);
        static DefaultCommandData parse_get_low_(SafeString& sms_string, Phone& source_phone);
        static DefaultCommandData parse_get_alarm_(SafeString& sms_string, Phone& source_phone);
        static DefaultCommandData parse_set_low_(SafeString& sms_string, Phone& source_phone);
        static DefaultCommandData parse_set_alarm_(SafeString& sms_string, Phone& source_phone);
        static DefaultCommandData parse_set_mode_(SafeString& sms_string, Phone& source_phone);
        static DefaultCommandData parse_set_mode_sim_(SafeString& sms_string, Phone& source_phone);
        static DefaultCommandData parse_set_mode_chip_(SafeString& sms_string, Phone& source_phone);

};