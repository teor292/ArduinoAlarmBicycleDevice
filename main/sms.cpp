#include "sms.h"

namespace
{
  bool isdigit(char c)
  {
    return c >= '0' && c <= '9';
  }

  bool check_next_two_is_digits(SafeString& str, int index)
  {
      if (!isdigit(str[index])) return false;
      if (!isdigit(str[index + 1])) return false;
      return true;
  }

  int count_new_lines_plus_one(const char *text)
  {
    int count = 1;
    createSafeStringFromCharPtr(tmp_str, const_cast<char*>(text));
    int index = -1;
    while (true)
    {
      index = tmp_str.indexOf('\n', index + 1);
      if (-1 == index) break;

      ++count;
    }
    return count;
  }
}

Sms::Sms(SoftwareSerial &serial, BlockTimeReader& reader, SafeString& g_string) 
 : serial_(serial),
   reader_(reader),
   g_string_(g_string)
{}

#if defined(CMGR)
bool Sms::ReadFromSerial(SafeString& cmti)
{
    PRINTLN(F("SMS GET"));
    int coma_index = cmti.indexOf(',');
    if (coma_index == -1) return false;
    createSafeString(sms_index, 4);
    cmti.substring(sms_index, coma_index + 1);
    sms_index.trim();
    int real_sms_index = 0;
    if (!sms_index.toInt(real_sms_index)) return false;
    PRINT(F("Sms index "));
    PRINTLN(real_sms_index);

    serial_.print("AT+CMGR=");
    serial_.println(real_sms_index);

    //read AT+CMGR=<index>
    if (!reader_.ReadLine(cmti, 5000)) return false;
    PRINT(cmti);

    //read sms header
    if (!reader_.ReadLine(cmti, 5000)) return false;
    PRINT(cmti);

    //max size with " is 18
    createSafeStringFromCharArray(quoted_phone_number, phone_);
    if (!extract_phone_number_(cmti, quoted_phone_number)) return false;
    PRINTLN(quoted_phone_number);

    createSafeStringFromCharArray(date_str, date_);
    if (!extract_date_(cmti, date_str, 3))
    {
      PRINT(F("FAILED GET DATE"));
    }
    PRINTLN(date_str);
    
    //read sms text
    createSafeStringFromCharArray(sms_text, text_);
    if (!reader_.ReadLine(sms_text, 5000)) return false;
    PRINT(sms_text);

    //read empty line
    if (!reader_.ReadLine(cmti, 500)) return false;
    PRINT(cmti);

    //read OK response
    if (!reader_.ReadLine(cmti, 500)) return false;
    PRINT(cmti);

    return true;
}

bool Sms::extract_phone_number_(SafeString& source, SafeString& dst)
{
    if (!source.startsWith("+CMGR")) return false;
    int index = source.indexOf(',');
    if (-1 == index) return false;
    int last_index = source.indexOf(',', index + 1);
    if (-1 == last_index) return false;
    source.substring(dst, index + 1, last_index);
    return true;
}

void Sms::DeleteReadIncomeMessages()
{
  serial_.println("AT+CMGD=1,1");
  if (!reader_.ReadLine(g_string_, 5000))
  {
    PRINTLN(F("AT+CMGD not found"));
    return;
  }
  PRINTLN(F("CMGD::::"));
  PRINTLN(g_string_);
  PRINTLN(F("returned from delete income"));

  if (!reader_.ReadLine(g_string_, 25000))
  {
    PRINTLN(F("status not found"));
    return;
  }
  PRINT(g_string_);
}

#endif

#ifdef CMT

bool Sms::TryReadForwardSmsFromSerial(SafeString& result)
{
  if (!reader_.ReadLine(result, 1000)) return false;
  if (!result.startsWith("CMT")) return false;
  if (!reader_.NClReadLine(result, 1000)) return false;
  PRINTLN(result);
  createSafeStringFromCharArray(quoted_phone_number, phone_);
  if (!cmt_extract_phone_number_(result, quoted_phone_number)) return false;
  PRINTLN(quoted_phone_number);

  createSafeStringFromCharArray(date_str, date_);
  if (!extract_date_(result, date_str, 2))
  {
      PRINT(F("!DATE"));
  }
    PRINTLN(date_str);
    
  auto index = result.indexOf((char)10);
  if (-1 == index)
  {
    PRINTLN("!LF");
    return false;
  }

  //read sms text
  createSafeStringFromCharArray(sms_text, text_);
  result.substring(sms_text, index + 1);
  PRINT(sms_text);

  return true;
}

bool Sms::cmt_extract_phone_number_(SafeString& source, SafeString& dst)
{
    int index = source.indexOf(':');
    if (-1 == index) return false;
    int last_index = source.indexOf(',', index + 1);
    if (-1 == last_index) return false;
    source.substring(dst, index + 2, last_index);
    return true;
}
#endif

bool Sms::extract_date_(SafeString& source, SafeString& dst, unsigned char count_comas_omit)
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
    for (int i = 0; i < 2; ++i, offset += 3)
    {
      if (!check_next_two_is_digits(source, index + offset)) return false;

      if ('/' != source[index + offset + 2]) return false;
    }
    if (!check_next_two_is_digits(source, index + offset)) return false;
    offset += 2;
    if (',' != source[index + offset]) return false;
    ++offset;
    for (int i = 0; i < 2; ++i, offset += 3)
    {
      if (!check_next_two_is_digits(source, index + offset)) return false;

      if (':' != source[index + offset + 2]) return false;
    }
    if (!check_next_two_is_digits(source, index + offset)) return false;

    source.substring(dst, index, index + offset + 2);

    return true;
}

const char* Sms::GetPhone() const
{
    return phone_;
}

const char* Sms::GetTime() const
{
    return date_;
}

char* Sms::GetText()
{
  return text_;
}

void Sms::SendSms(const char *text)
{
    int count_new_lines = count_new_lines_plus_one(text);
    serial_.print("AT+CMGS=");
    serial_.println(phone_);
    //read AT+CMGS=phone
    if (!reader_.ReadLine(g_string_, 1000))
    {
      PRINTLN(F("AT not found"));
      return;
    }
    PRINTLN(F("AT::::"));
    PRINT(g_string_);
    char enter_symbol;
    if (!reader_.ReadChar(enter_symbol, 1000)) 
    {
      PRINTLN(F("no found23"));
      return;
    }
    if ('>' != enter_symbol)
    {
        PRINTLN(F("no found > symbol"));
        return;
    }
    serial_.print(text);

    serial_.write(26);


    //read text
    if (!reader_.ReadLine(g_string_, 60000)) 
    {
      PRINT(F("text not found"));
      return;
    }
    PRINT(F("Text from answer: "));
    PRINT(g_string_);
    for ( int i = 1; i < count_new_lines; ++i)
    {
      //read additional text
      if (!reader_.ReadLine(g_string_, 60000)) 
      {
        PRINT(F("additional text not found"));
        return;
      }
      PRINT(F("Additional Text from answer: "));
      PRINT(g_string_);
    }


    //read cmgs line
    if (!reader_.ReadLine(g_string_, 60000)) 
    {
      PRINT(F("cmgs not found"));
      return;
    }
    PRINT(F("CMGS TEXT: "));
    PRINT(g_string_);

    //read empty line
    if (!reader_.ReadLine(g_string_, 1000))
    {
          PRINT(F("empty line not found2"));
          return;
    }
    //read status
    if (!reader_.ReadLine(g_string_, 2000))
    {
          PRINT(F("sttaus line not found2"));
          return;
    }
    PRINT(g_string_);
}


void Sms::SetPhone(SafeString& phone)
{
  createSafeStringFromCharArray(tmp, phone_);
  tmp = phone;
}