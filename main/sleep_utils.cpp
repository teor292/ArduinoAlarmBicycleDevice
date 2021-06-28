#include "sleep_utils.h"
#include "BatteryChecker.h"
#include "header.h"

#if defined(__SAMD21G18A__)

#include "SAMDLowPower.h"
bool attached_interrupt = false;
#else

#include "LowPower.h"

#endif

//flag indicating INT0
volatile bool f_extern_interrupt = false;

void int0_func()
{
  f_extern_interrupt = true;
  PRINT("INT");
}

void go_to_sleep(BatteryChecker& checker)
{
    #if defined(__SAMD21G18A__)

    //  pinMode(13,OUTPUT);  //led RX
   //  digitalWrite(13,LOW);

    if (!attached_interrupt)
    {
      SAMDLowPower::SetAwakeCallback(AWAKE_SIM800_PIN, int0_func, FALLING);
      //LowPower.attachInterruptWakeup(AWAKE_SIM800_PIN, int0_func, FALLING);
      attached_interrupt = true; 
    }

    for ( int i = 0; i < 60 && !f_extern_interrupt; ++i)
    {
      SAMDLowPower::SleepFor(60000);
        //LowPower.sleep(60000);
        //checker.AddToRealTime(60000);
    }

    //digitalWrite(13,HIGH);

    #else

    attachInterrupt(0, int0_func, FALLING);

    //awake 1 per hour
    for (int i = 0; i < 3600 / 8 && !f_extern_interrupt; ++i)
    {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

      //millis don't work during power down, so add it
      checker.AddToRealTime(8 * 1000);   
    }

    detachInterrupt(0);

    #endif


}