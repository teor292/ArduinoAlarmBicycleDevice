#include "SAMDLowPower.h"

#if defined(__SAMD21G18A__)

#include <Arduino.h>
#include "header.h"

void SAMDLowPower::Initialize()
{
    RTCMode0::Initialize();
}

void SAMDLowPower::SetAwakeTimeCallback(InterruptCallback callback)
{
    RTCMode0::SetIntCallback(callback);
}

bool SAMDLowPower::SetAwakeCallback(uint8_t pin, InterruptCallback callback, int mode)
{
     EExt_Interrupts in = g_APinDescription[pin].ulExtInt;
	 if (in == NOT_AN_INTERRUPT || in == EXTERNAL_INT_NMI)
	 	return false;

	//pinMode(pin, INPUT_PULLUP);
	attachInterrupt(pin, callback, mode);
	//don't know why need reconfigure GCLK after each interrupt
	//but if configure in initialize and not here -> SAMD not awake by interrupt
	configure_gclk6_();
	// Enable wakeup capability on pin in case being used during sleep
	EIC->WAKEUP.reg |= (1 << in);

    return true;
}

void SAMDLowPower::UnsetAwakeCallback(uint8_t pin)
{
	EExt_Interrupts in = g_APinDescription[pin].ulExtInt;
	if (in == NOT_AN_INTERRUPT || in == EXTERNAL_INT_NMI) return;

	detachInterrupt(pin);
}

void SAMDLowPower::SleepFor(uint32_t time)
{
    auto tm = RTCMode0::Time();
    tm += time;
    RTCMode0::SetIntTime(tm);

    Sleep();
}

void SAMDLowPower::SleepTo(uint32_t time)
{
    RTCMode0::SetIntTime(time);

    Sleep();   
}

//copied from ArduinoLowPower
void SAMDLowPower::Sleep()
{
    bool restoreUSBDevice = false;
	if (SERIAL_PORT_USBVIRTUAL) 
    {
		USBDevice.standby();
	} else 
    {
		USBDevice.detach();
		restoreUSBDevice = true;
	}
	// Disable systick interrupt:  See https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;	
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	__DSB();
	__WFI();
	// Enable systick interrupt
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;	
	if (restoreUSBDevice) 
    {
		USBDevice.attach();
	}
}


void SAMDLowPower::configure_gclk6_()
{
	PRINTLN(F("configure_gclk6_"));

	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK6 | GCLK_CLKCTRL_ID( GCM_EIC )) ;  //EIC clock switched on GCLK6
	while (GCLK->STATUS.bit.SYNCBUSY);

	//GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(6));  //source for GCLK6 is OSCULP32K
	GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_ID(6));
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

	GCLK->GENCTRL.bit.RUNSTDBY = 1;  //GCLK6 run standby
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

	/* Errata: Make sure that the Flash does not power all the way down
     	* when in sleep mode. */

	NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;
}

#endif