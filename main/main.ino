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
#include "BookReader.h"

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
BookReader adminer(SIM800, reader);

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
const char CMT_MODE[] = "AT+CNMI=1,2,0,0,0";
const char SILINCE_MODE[] = "AT+CNMI=0,0,0,0,0";

bool get_signal_strength(SafeString& str);
void perform_command(const char* command);

void setup() 
{
  Serial.begin(GPSBaud);            
  PRINTLN(F("Start!"));
  SafeString::setOutput(Serial);

  SIM800.begin(GPSBaud);      

  perform_command("AT");

  PRINTLN(F("AT ANSWER GET"));

  //wait 10 seconds for initialization of SIM800L is complete
  //if continue without delay -> multiple unsolicit result codes (URC) will come 
  delay(10000); 

  //it would be some lines after initialization in buffer:
  //0. ??? Well once I this smth, but not remember what :)
  //1. +CFUN: 1
  //2. +CPIN: READY
  //3. Call Ready
  //4. SMS Ready
  //read it without check
  for ( unsigned char i = 0; i < 5; ++i)
  {
    reader.ReadLine(test_string, 1000);
  }     

  //configure text mode
  perform_command("AT+CMGF=1");

  //prohibit income calls
  perform_command("AT+GSMBUSY=1");

  //will get sms and send it directly to software
  //without saving to sms memory
  while (!set_sms_mode(CMT_MODE))
  {
    delay(1000);
  }

  //load admin phone
  while (!adminer.LoadAdminPhone(test_string))
  {
    delay(1000);
  }

#ifdef DEBUG

  if (adminer.IsEmpty())
  {
    Serial.println(F("Phone is empty"));
    return;
  }
  Serial.println(adminer.GetAdminPhone());

#endif
}

void perform_command(const char* command)
{
  while (true)
  {
    SIM800.println(command);
    if (reader.ReadStatusResponse(test_string, 5000))
    {
      int index = test_string.indexOf(OK);
      if (-1 != index)
      {
        break;
      } 
      PRINTLN(F("cmd not OK"));
      delay(1000);
      continue;
    }
    PRINTLN(F("NO SIM800 ANSWER"));
  }
}

bool set_sms_mode(const char *mode)
{
  SIM800.println(mode);
  if (!reader.ReadUntil(test_string, 1000, mode))
  {
    PRINTLN(F("Read until failed"));
    return false;
  }
  if (!reader.ReadLine(test_string, 1000)) return false;

  return test_string.startsWith(OK);
}

void loop() 
{
  if (!adminer.IsEmpty())
  {
    if (vibro.Update())
    {
      sms_one.SetPhone(adminer.GetAdminPhone());
      sms_one.SendSms("!!! Vibro alarm !!!");
    }
  }


  if (!adminer.IsEmpty()
    && send_alarm_on_low_battery
    && battery_checker.Check()
    && battery_checker.Update())
  {
      sms_one.SetPhone(adminer.GetAdminPhone());
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
    if (!set_sms_mode(SILINCE_MODE))
    {
      PRINTLN(F("!sms mode"));
      return;
    }

    EXIT_SCOPE_SIMPLE(
      sms_one.DeleteAllSms(test_string);
      set_sms_mode(CMT_MODE);
      );

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
      adminer.SetAdminPhone(sms_one.GetPhone());
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
    //if smth unknown -> do nothing
    //sms_one.SendSms("SMSTEXT");
    //PRINTLN(F("Returned from send sms"));
      
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
