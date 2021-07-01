#include "sms.h"

Sms::Sms(Stream &serial, BlockTimeReader& reader, SafeString& g_string) 
 : serial_(serial),
   reader_(reader),
   g_string_(g_string)
{}



void Sms::DeleteAllSms(SafeString& buffer)
{
  buffer.clear();
  serial_.println("AT+CMGDA=\"DEL ALL\"");
  //nothing to do with error
  reader_.ReadStatusResponse(buffer, 25000);
}



void Sms::SendSms(const char *text)
{
  //Sometimes an error occurs when send.
  //The reason of such behaviour is not clear. So try to send with 2 attempts.
  for (int i = 0; i < 2; ++i)
  {
    if (send_sms_one_(text)) return;
  }
}
bool Sms::sens_sms_one_(const char *text)
{
    static const char AT_CMGS[] = "AT+CMGS=";
   // int count_new_lines = count_new_lines_plus_one(text);
    serial_.print(AT_CMGS);
    serial_.println(phone_.phone);

    //read >
    if (!reader_.ReadUntil(g_string_, 6000, '>'))
    {
      PRINTLN(F("> not found"));
      //if not found -> not return, continue
      //because sim800L can wait for text and symbol 26
      //If sim800L will wait -> we can't enter other commands
      //return;
    }
    PRINTLN(F("AT::::"));
    PRINT(g_string_);

    serial_.print(text);

    serial_.write(26);

    if (!reader_.ReadStatusResponse(g_string_, 60000))
    {
        PRINTLN(F("FRSMSRP"));
        PRINTLN(g_string_);
        return false;
    }
    PRINTLN(g_string_);

    auto index = g_string_.indexOf("OK");
    if (-1 == index)
    {
      PRINTLN(F("SMS FAIL"));
      return false;
    }

    PRINTLN(F("SMS OK"));
    return true;
}


void Sms::SetPhone(const char* phone)
{
  phone_ = phone;
}