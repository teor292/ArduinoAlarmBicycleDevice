#include "SmsReader.h"
#include "DefaultCommandParser.h"
#include "GPSCommandParser.h"
#include "scope_exit.h"

namespace
{
    template<typename T, int COUNT = 2>
    bool convert_to_number(SafeString& source, int index, T& result)
    {
        createSafeString(tmp, COUNT);
        source.substring(tmp, index, index + COUNT);
        int res;
        //must never occured
        if (!tmp.toInt(res)) return false;
        result = static_cast<T>(res);
        return true;
    }
}

bool SmsReader::Write(char c)
{
    return buffer_.Write(c);
}

bool SmsReader::IsFilled() const
{
    return buffer_.IsFilled();
}

SmsData SmsReader::Work()
{
    if (!buffer_.IsFilled()) return SmsData{};
    auto &buf = buffer_;
    EXIT_SCOPE([&buf]()
    {
        buf.Clear();
    });
    auto& result = buffer_.GetSms();
    

    PRINTLN(result);
    Phone phone;
    createSafeStringFromCharArray(quoted_phone_number, phone.phone);
    if (!cmt_extract_phone_number_(result, quoted_phone_number)) return SmsData{};
    PRINTLN(quoted_phone_number);

    DateTime date;
    extract_date_(result, date, 2);
        
    auto index = result.indexOf((char)10);
    if (-1 == index) return SmsData{};

    //read sms text
    createSafeString(sms_text, SMS_BUF - 20);
    result.substring(sms_text, index + 1);
    PRINT(F("SMS TEXT: "));
    PRINT(sms_text);

    return parse_command_(sms_text, date, phone);

}

SmsData SmsReader::parse_command_(SafeString& sms_string, DateTime& date, Phone& phone)
{
    #if defined(GPS)
    createSafeString(sms_string_2, SMS_BUF - 20);
    sms_string_2 = sms_string;
    #endif
    auto default_command = DefaultCommandParser::ParseSms(sms_string, phone);
    if (DEFAULT_COMMANDS::INVALID != default_command.cmd)
    {
        default_command.date_time = date;
        return SmsData{default_command};
    }
    #if defined(GPS)
    
    auto gps_command = GPSCommandParser::ParseSms(sms_string_2, phone);
    if (GPS_COMMANDS::INVALID != gps_command.cmd)
    {
        return SmsData{gps_command};
    }
    #endif

    return SmsData{};
}

bool SmsReader::extract_date_(SafeString& source, DateTime& dst, unsigned char count_comas_omit)
{
 //+CMGR: "REC UNREAD","<phone>","","21/04/27,18:45:05+12"
    //+CMT: "<phone>","","21/05/01,13:44:00+12"
    int index = -1;
    for ( int i = 0; i < count_comas_omit; ++i)
    {
        index = source.indexOf(',', index + 1);
        if (-1 == index) return false;
    }
    //next must be quote
    ++index;
    if ('\"' != source[index]) return false;

    ++index;
    int offset = 0;

    if ('/' != source[index + offset + 2]) return false;

    if (!convert_to_number(source, index + offset, dst.year)) return false;


    offset += 3;

    if ('/' != source[index + offset + 2]) return false;

    if (!convert_to_number(source, index + offset, dst.month)) return false;


    offset += 3;

    if (!convert_to_number(source, index + offset, dst.day)) return false;

    offset += 2;
    if (',' != source[index + offset]) return false;
    ++offset;

    if (':' != source[index + offset + 2]) return false;

    if (!convert_to_number(source, index + offset, dst.hours)) return false;

    offset += 3;

    if (':' != source[index + offset + 2]) return false;

    if (!convert_to_number(source, index + offset, dst.minutes)) return false;

    offset += 3;

    if (!convert_to_number(source, index + offset, dst.seconds)) return false;

    return true;
}

bool SmsReader::cmt_extract_phone_number_(SafeString& source, SafeString& dst)
{
    int index = source.indexOf(':');
    if (-1 == index) return false;
    int last_index = source.indexOf(',', index + 1);
    if (-1 == last_index) return false;
    source.substring(dst, index + 2, last_index);
    return true;
}