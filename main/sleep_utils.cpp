#include "sleep_utils.h"
#include "header.h"

#if defined(__SAMD21G18A__)

#include "SAMDLowPower.h"
//#include "ArduinoLowPower.h"
//bool attached_interrupt = false;
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

void go_to_sleep(BatteryChecker& checker, uint32_t current_time, uint32_t sleep_for_time)
{
    #if defined(__SAMD21G18A__)


    if (f_extern_interrupt) return;


     //pinMode(13,OUTPUT);  //led RX
    // digitalWrite(13,LOW);

    //if int occures after above line and before sleep -> it will be ignored
    SAMDLowPower::SleepTo(current_time + sleep_for_time);

   // digitalWrite(13,HIGH);

    #else

    //attachInterrupt(0, int0_func, FALLING);

    int count = sleep_for_time / 8;
    for (int i = 0; i < count && !f_extern_interrupt; ++i)
    {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

      //millis don't work during power down, so add it
      checker.AddToRealTime(8 * 1000);   
    }

    //detachInterrupt(0);

    #endif


}