#include <BufferedInput.h>
#include <BufferedOutput.h>
#include <loopTimer.h>
#include <millisDelay.h>
#include <SafeString.h>
#include <SafeStringReader.h>
#include <SafeStringStream.h>

#include <SoftwareSerial.h>
#include "header.h"
#include "BlockTimeReader.h"
#include "BatteryReader.h"
#include "BatteryChecker.h"
#include "sms.h"
#include "scope_exit.h"
#include "VibroStater.h"

static const int RXPin = 8, TXPin = 9;
static const uint32_t GPSBaud = 9600;

SoftwareSerial SIM800(RXPin, TXPin);        // 8 - RX Arduino (TX SIM800L), 9 - TX Arduino (RX SIM800L)

createSafeString(test_string, 200);
millisDelay time_delay;


BlockTimeReader reader(SIM800, time_delay);
BatteryReader battery(test_string, SIM800, reader);
BatteryChecker battery_checker(battery);
Sms sms_one(SIM800, reader, test_string);

VibroStater vibro(5);

createSafeString(admin_phone_number, 18);
unsigned char send_alarm_on_low_battery = 1;

const char GET_BATTERY[] = "get battery";
const char GET_TIME[] = "get time";
const char GET_SIGNAL[] = "get signal";
const char SET_ADMIN[] = "set admin";
const char LOW_BATTERY[] = "low battery ";
const char SET_ALARM[] = "set alarm ";
const char SENSITY[] = "sensity ";
const char ON[] = "on";
const char OFF[] = "off";
const char ERROR[] = "Error";
const char OK[] = "OK";

bool get_signal_strength(SafeString& str);

void setup() 
{
  Serial.begin(GPSBaud);            
  PRINTLN(F("Start!"));
  SafeString::setOutput(Serial);


  
  SIM800.begin(GPSBaud);            

  while (true)
  {
    SIM800.println("AT");
    if (reader.ReadStatusResponse(test_string, 5000)) break;
    PRINTLN(F("NO SIM800 ANSWER"));
  }

  int index = test_string.indexOf(OK);
  if (-1 == index)
  {
    PRINTLN(F("AT not OK"));
    while (true); //stop here
  }



  PRINTLN(F("ANSWER GET"));
  //configure text mode
  SIM800.println("AT+CMGF=1");
  reader.ReadStatusResponse(test_string, 1000);
  //response must be OK, so not check it

  SIM800.println("AT+GSMBUSY=1");
  reader.ReadStatusResponse(test_string, 1000);

  //will get sms and send it directly to software
  //withour saving to sms memory
  SIM800.println("AT+CNMI=1,2,0,0,0");
  reader.ReadStatusResponse(test_string, 1000);
}

void loop() 
{
  if (!admin_phone_number.isEmpty())
  {
    if (vibro.Update())
    {
      sms_one.SetPhone(admin_phone_number);
      sms_one.SendSms("!!! Vibro alarm !!!");
    }
  }


  if (!admin_phone_number.isEmpty()
    && send_alarm_on_low_battery
    && battery_checker.Check()
    && battery_checker.Update())
  {
      sms_one.SetPhone(admin_phone_number);
      sms_one.SendSms(battery_checker.GetData());
  }

  if (SIM800.available())
  {
    char c = (char)SIM800.read();
    if (c != '+')
    {
      WRITE(c);
      return;
    }
    if (!sms_one.TryReadForwardSmsFromSerial(test_string)) 
    {
      PRINTLN(F("Error read CMT sms"));
      return;
    }

    char *text = sms_one.GetText();
    createSafeStringFromCharPtr(sms_text, text);
    sms_text.toLowerCase();
    sms_text.trim();

    if (sms_text == GET_BATTERY)
    {
        if (!battery.ReadBattery())
        {
          sms_one.SendSms(ERROR);
        } else
        {
          sms_one.SendSms(battery.GetData());
        }
        return;
    }
    if (sms_text == GET_TIME)
    {
      sms_one.SendSms(sms_one.GetTime());
      return;
    }
    if (sms_text == GET_SIGNAL)
    {
      createSafeString(signal_str, 10);
      if (get_signal_strength(signal_str))
      {
        sms_one.SendSms(signal_str.c_str());
      } else
      {
        sms_one.SendSms(ERROR);
      }
      return;
    }
    if (sms_text == SET_ADMIN)
    {
      admin_phone_number = sms_one.GetPhone();
      sms_one.SendSms(OK);
      return;
    }
    if (sms_text.startsWith(LOW_BATTERY))
    {
        const char* tmp = sms_text.c_str() + sizeof(LOW_BATTERY) - 1;
        createSafeStringFromCharPtr(tmp_str, const_cast<char*>(tmp));
        if (tmp_str == ON)
        {
          send_alarm_on_low_battery = 1;
          sms_one.SendSms(OK);
        } 
        else if (tmp_str == OFF)
        {
          send_alarm_on_low_battery = 0;
          sms_one.SendSms(OK);
        }
        return;
    }
    if (sms_text.startsWith(SET_ALARM))
    {
        const char* tmp = sms_text.c_str() + sizeof(SET_ALARM) - 1;
        createSafeStringFromCharPtr(tmp_str, const_cast<char*>(tmp));
        if (tmp_str.startsWith(SENSITY))
        {
          const char* tmp2 = tmp_str.c_str() + sizeof(SENSITY) - 1;
          createSafeStringFromCharPtr(sensity_str, const_cast<char*>(tmp2));
          int sensity = 0;
          if (sensity_str.toInt(sensity))
          {
            vibro.SetCountChanges(static_cast<unsigned int>(sensity));
            sms_one.SendSms(OK);
          } else
          {
            sms_one.SendSms(ERROR);
          }
          return;
        }
        if (tmp_str == ON)
        {
          vibro.EnableAlarm(1);
          sms_one.SendSms(OK);
        }
        else if (tmp_str == OFF)
        {
          vibro.EnableAlarm(0);
          sms_one.SendSms(OK);
        }
        else
        {
          sms_one.SendSms(ERROR);
        }
        return;
    }
    
    sms_one.SendSms("SMSTEXT");
    PRINTLN(F("Returned from send sms"));
      
  }
}


bool get_signal_strength(SafeString& str)
{
  SIM800.println("AT+CSQ");

  if (!reader.ReadSomeResponse(test_string, 500))
  {
    //Serial.println("Error read report from signal strength");
    return false;
  }
  //Serial.print("Read report: ");
 // Serial.print(test_string);
 // Serial.print("Read report end");
  if (!test_string.startsWith("+CSQ: ")) return false;

  test_string.substring(str, 6, 8);

  str += " (0-31)";
  return true;

}