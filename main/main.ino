#include <BufferedInput.h>
#include <BufferedOutput.h>
#include <loopTimer.h>
#include <millisDelay.h>
#include <SafeString.h>
#include <SafeStringReader.h>
#include <SafeStringStream.h>

#include "header.h"
#include "BlockTimeReader.h"
#include "BatteryReader.h"
#include "BatteryChecker.h"
#include "sms.h"
#include "scope_exit.h"
#include "VibroStater.h"
#include "BookReader.h"
#include "Settings.h"
#include "work_mode.h"
#include "ModeSerial.h"
#include "sleep_utils.h"
#include "MillisCallback.h"
#include "GPSWorker.h"

//#define SIM800_INITIALIZATION

//if change baud rate -> change command AT+IPR!!!
const uint32_t GPSBaud = 9600;

//with samd this is Uart on SERCOM2 (disable I2C)
ModeSerial SIM800; 

#if defined(__SAMD21G18A__)

void SERCOM2_Handler()
{
  SIM800.IrqHandler();
}

#endif

//this callback is neccessary for retreiving gps
//data while SIM800 work (send sms, etc.)
void wait_callback_for_gps_read();


createSafeString(test_string, 200);

MillisReadDelay time_delay(SIM800, wait_callback_for_gps_read);


BlockTimeReader reader(time_delay);
BatteryReader battery(test_string, SIM800, reader);
BatteryChecker battery_checker(battery);
Sms sms_one(SIM800, reader, test_string);

VibroStater vibro(VIBRO_PIN);
BookReader adminer(SIM800, reader);




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
const char SET_MODE[] = "set mode ";
const char SLEEP_MODE[] = "sleep";
const char DEFAULT_MODE[] = "def";
const char SLEEP_MODE_COMMAND[] = "AT+CSCLK=2";
const char DEFAULT_MODE_COMMAND[] = "AT+CSCLK=0";
const char PROHIBIT_CALLS[] = "AT+GSMBUSY=1";
const char VIBRO_ALARM[] = "!!! Vibro alarm !!!";

bool get_signal_strength(SafeString& str);
void perform_command(const char* command);
void set_alarm_and_sms(unsigned char value);
bool perform_sim800_command(const char *cmd);
bool detect_sim800_mode_and_set_def();

bool send_alarm_on_low_battery = true;
Settings settings;

//time when arduino was in sleep mode last time
unsigned long last_enter_sleep_time = 0;

bool was_in_sleep_mode = false;

#if defined(GPS)


const char GET_GPS[] = "get gps";

//this callback is neccessary for retreiving sms
//data while GPS work
void wait_callback_for_sms_read()
{
  //TODO
}

GPSWorker gps_worker(Serial1, sms_one, wait_callback_for_sms_read);

#endif


void wait_callback_for_gps_read()
{
  #if defined(GPS)
  gps_worker.Read();
  #endif
}

void setup() 
{
  //configure here because sim800l due to lack of amperage can
  //reboot while initialization if D2 (INT0) in in default mode
  pinMode(AWAKE_SIM800_PIN, INPUT); 

  #if defined(DEBUG)
  Serial.begin(GPSBaud); 

  #if defined(__SAMD21G18A__)
  //wait initialization
  while (!Serial);           
  #endif
  
  PRINTLN(F("Start!"));
  SafeString::setOutput(Serial);

  #endif

  settings.Load();
  vibro.EnableAlarm(settings.alarm);

  SIM800.begin(GPSBaud); 

  //if sim800 in sleep mode and we use standart ->
  //we get nothing. But if it in standart mode and
  //we use sleep -> everything will be OK
  SIM800.SetMode(WORK_MODE::SLEEP);     

  perform_command("AT");
 // Serial1.

  PRINTLN(F("AT ANSWER GET"));

  //wait 10 seconds for initialization of SIM800L is complete
  //if continue without delay -> multiple unsolicit result codes (URC) will come 
  delay(10000); 

  //it would be some lines after initialization in buffer:
  //0. RDY
  //1. +CFUN: 1
  //2. +CPIN: READY
  //3. Call Ready
  //4. SMS Ready
  //read it without check
  //if sim800 already worker -> read empty lines
  for ( unsigned char i = 0; i < 5; ++i)
  {
    reader.ReadLine(test_string, 1000);
  }     
  //detect current mode and set default
  detect_sim800_mode_and_set_def();

  #if defined(SIM800_INITIALIZATION)

  //configure text mode
  perform_command("AT+CMGF=1");

  //save baud rate
  perform_command("AT+IPR=9600");

  //will get sms and send it directly to software
  //without saving sms to sim card memory
  while (!set_sms_mode(CMT_MODE))
  {
    delay(1000);
  }

  perform_command("AT&W");

  #endif

  //prohibit income calls
  perform_command(PROHIBIT_CALLS);

  //load admin phone
  adminer.LoadAdminPhone(test_string);

#ifdef DEBUG

  if (adminer.IsEmpty())
  {
    Serial.println(F("Phone is empty"));
    return;
  }
  Serial.println(adminer.GetAdminPhone());

#endif
}

void wait_for_ok();

bool detect_sim800_mode_and_set_def()
{
    //read mode
    SIM800.println(F("AT+CSCLK?"));
    
    if (!reader.ReadUntil(test_string, 1000, "+CSCLK:"))
    {
      PRINTLN(F("FRM"));
      return false;
    }
    test_string.substring(test_string, 8, 9);
    if (test_string == "0")
    {
      SIM800.SetMode(WORK_MODE::STANDART);
      PRINTLN(F("STM"));
      wait_for_ok();
    } 
    else
    {
      PRINTLN(F("SLM"));
      wait_for_ok();
      clear_buffer();
      if (perform_sim800_command(DEFAULT_MODE_COMMAND))
      {
          SIM800.SetMode(WORK_MODE::STANDART);
      }

    }

     

    clear_buffer();



    return true;
}

void wait_for_ok()
{
    int index = test_string.indexOf(OK);
    if (-1 == index)
    {
      reader.ReadUntil(test_string, 1000, OK);
    }
}


void perform_command(const char* command)
{
  while (true)
  {
    PRINTLN("CMD:::");
    PRINTLN(command);
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
  if (!reader.ReadStatusResponse(test_string, 5000))
  {
    PRINTLN(F("Read until failed"));
    PRINTLN(test_string);
    return false;
  }

  return -1 != test_string.indexOf(OK);
}

void clear_buffer()
{
  while (SIM800.available())
  {
    SIM800.read();
  }
}

void do_vibro()
{
  if (!adminer.IsEmpty()
    && !SIM800.IsSleepMode()
    && vibro.Update())
  {
      if (!set_sms_mode(SILINCE_MODE))
      {
        PRINTLN(F("!sms mode"));
        return;
      }
      clear_buffer();
      sms_one.SetPhone(adminer.GetAdminPhone());
      sms_one.SendSms(VIBRO_ALARM);
      set_sms_mode(CMT_MODE);
  }
}

void do_battery()
{
  if (!f_extern_interrupt
    && !adminer.IsEmpty()
    && send_alarm_on_low_battery
    && battery_checker.Check()
    && battery_checker.Update())
  {
      if (!set_sms_mode(SILINCE_MODE))
      {
        PRINTLN(F("!sms mode"));
        return;
      }
      clear_buffer();
      sms_one.SetPhone(adminer.GetAdminPhone());
      sms_one.SendSms(battery_checker.GetData());
      set_sms_mode(CMT_MODE);
  }
}



void loop() 
{
  EXIT_SCOPE_SIMPLE(

    Serial.flush();  
    f_extern_interrupt = false;
    was_in_sleep_mode = false;



    //call check battery here
    //because it is neccessary check battery
    //when f_extern_interrupt is 0
    //microcontroller can awake not by timer
    //but by sms, so it must read sms before check battery
    do_battery();

    if (!SIM800.IsSleepMode()) return;

    auto current_time = millis();
    if (current_time - last_enter_sleep_time < 5000) return;
    last_enter_sleep_time = current_time;

    go_to_sleep(battery_checker);

    //reset last awake time because millis don't work while sleep
    SIM800.ResetTime();


    was_in_sleep_mode = true;


    PRINTLN(F("AWAKE"));
  );

  do_vibro();

 


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
    //wait for full awake before send data
    if (was_in_sleep_mode) delay(100);
    if (!set_sms_mode(SILINCE_MODE))
    {
      PRINTLN(F("!sms mode"));
      return;
    }
    //clear buffer if it contains some data
    clear_buffer();
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
          //when alarm enable -> disable sleep mode
          bool ok = WORK_MODE::SLEEP != SIM800.GetMode();
          if (WORK_MODE::SLEEP == SIM800.GetMode())
          {
            if (perform_sim800_command(DEFAULT_MODE_COMMAND))
            {
              PRINTLN(F("SMST"));
              SIM800.SetMode(WORK_MODE::STANDART);
              ok = true;
            }
          }
          if (ok)
          {
            set_alarm_and_sms(1);
          } else
          {
            sms_one.SendSms(ERROR);
          }


        }
        else if (tmp_str == OFF)
        {
          set_alarm_and_sms(0);
        }
        else
        {
          sms_one.SendSms(ERROR);
        }
        return;
    }

    if (sms_text.startsWith(SET_MODE))
    {
        const char* tmp = sms_text.c_str() + sizeof(SET_MODE) - 1;
        createSafeStringFromCharPtr(tmp_str, const_cast<char*>(tmp));
        if (tmp_str == SLEEP_MODE)
        {
          if (perform_sim800_command(SLEEP_MODE_COMMAND))
          {
            vibro.EnableAlarm(0); //disable alarm in sleep mode
            SIM800.SetMode(WORK_MODE::SLEEP);
            sms_one.SendSms(OK);
            last_enter_sleep_time = 0;
          }
        }
        else if (tmp_str == DEFAULT_MODE)
        {
          if (perform_sim800_command(DEFAULT_MODE_COMMAND))
          {
            SIM800.SetMode(WORK_MODE::STANDART);
            sms_one.SendSms(OK);
          } 
        }
        else
        {
          sms_one.SendSms(ERROR);
        }
        return;
    }
    #if defined(GPS)

    if (sms_text.startsWith(GET_GPS))
    {
      do_get_gps();
      return;
    }

    #endif
    //if smth unknown -> do nothing
      
  }
}

#if defined(GPS)

void do_get_gps()
{

}

#endif

bool perform_sim800_command(const char *cmd)
{
  SIM800.println(cmd);
  //don't know what to do with status code
  if (!reader.ReadStatusResponse(test_string, 1000)) return false;
  return -1 != test_string.indexOf(OK);
}

void set_alarm_and_sms(unsigned char value)
{
    settings.alarm = value;
    settings.Save();
    vibro.EnableAlarm(value);
    sms_one.SendSms(OK);
}


bool get_signal_strength(SafeString& str)
{
  SIM800.println("AT+CSQ");

  if (!reader.ReadStatusResponse(test_string, 1000)) return false;

  auto index = test_string.indexOf("+CSQ:");
  if (-1 == index) return false;

  test_string.substring(str, index + 6, index + 8);

  str += " (0-31)";
  return true;

}
