#include "RTCMode0.h"

#if defined(__SAMD21G18A__)

#include <Arduino.h>
#include "header.h"
#include "cmsis_gcc.h"

namespace
{
  /* Wait for sync in write operations */
  inline bool is_sync_rtc()
  {
    return (RTC->MODE0.STATUS.bit.SYNCBUSY);
  }

  InterruptCallback int_callback{nullptr};
}


void RTC_Handler(void)
{
  PRINTLN("Handler");
  PRINTLN(RTC->MODE0.INTFLAG.reg);
  if (nullptr != int_callback)
  {
    int_callback();
  }
  //clear flag. If not -> interrupt will continue
  RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0; 

}



void RTCMode0::Initialize()
{
    // turn on digital interface clock
    PM->APBAMASK.reg |= PM_APBAMASK_RTC; 

    //configure XOSC32K (not OSCULP32K because it is use ~ as much as XOSC32K power)
    config_32k_OSC_();
    PRINTLN(F("config_32k_OSC_"));

    configure_clock_();

    //disable for reconfiguring
    disable_rtc_();
    PRINTLN(F("disable_rtc_"));
    reset_rtc_();
    PRINTLN(F("reset_rtc_"));

    RTC->MODE0.READREQ.reg &= ~RTC_READREQ_RCONT; // disable continuously mode

    uint16_t tmp_reg = 0;
    tmp_reg |= RTC_MODE0_CTRL_MODE_COUNT32; // set clock operating mode
    tmp_reg |= RTC_MODE0_CTRL_PRESCALER_DIV256; // set prescaler for MODE0
    tmp_reg &= ~RTC_MODE0_CTRL_MATCHCLR; // disable clear on match

    RTC->MODE0.CTRL.reg = tmp_reg;
    
    while (is_sync_rtc());

    PRINTLN("is_sync_rtc1");

    NVIC_EnableIRQ(RTC_IRQn); // enable RTC interrupt 
    NVIC_SetPriority(RTC_IRQn, 0x00);

    //set max value to compare by default
    //this value will be reached after 30 years with DIV256 prescaler
    RTC->MODE0.COMP[0].bit.COMP = 0xFFFFFFFF;
    while (is_sync_rtc());

    //enable compare interrupt while RTC is disabled
    //Otherwise, the callback is called immediately after switching on. 
    //WTF???
    RTC->MODE0.INTENSET.bit.CMP0 = 1; 

    while (is_sync_rtc());
    PRINTLN(F("is_sync_rtc2"));
    
    reset_clear_rtc_();
    PRINTLN(F("reset_clear_rtc_"));
    
    enable_rtc_(); 
    PRINTLN(F("enable_rtc_"));
}

void RTCMode0::SetIntCallback(InterruptCallback callback)
{
    int_callback = callback;
}

uint32_t RTCMode0::Time()
{
    RTC->MODE0.READREQ.reg = RTC_READREQ_RREQ;
    while (is_sync_rtc());
    auto value = RTC->MODE0.COUNT.bit.COUNT;
    //Serial.println(value);
    //return (value * 1000) >> 3;
    return value;
}

 void RTCMode0::SetIntTime(uint32_t time)
 {
    //time <<= 3;
    //time /= 1000;

    PRINTLN(time);


    RTC->MODE0.COMP[0].bit.COMP = time;
    while (is_sync_rtc());
 }



void RTCMode0::enable_rtc_()
{
  RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_ENABLE; // enable RTC
  while (is_sync_rtc());
}

void RTCMode0::reset_clear_rtc_()
{
  RTC->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_SWRST; // software reset remove
  while (is_sync_rtc());
}

void RTCMode0::reset_rtc_()
{
  RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_SWRST; // software reset
  while (is_sync_rtc());
}

void RTCMode0::disable_rtc_()
{
  RTC->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_ENABLE; // disable RTC
  while (is_sync_rtc());
}

void RTCMode0::configure_clock_()
{
  //clock generator 2, divide 4 -> result 8196 frequency
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(2)|GCLK_GENDIV_DIV(4);

  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

  //divsel -> 1024 frequency
  GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );
  //GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_DIVSEL );
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

  GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));

  while (GCLK->STATUS.bit.SYNCBUSY);
}

void RTCMode0::config_32k_OSC_() 
{

  SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_ONDEMAND |
                         SYSCTRL_XOSC32K_RUNSTDBY |
                         SYSCTRL_XOSC32K_EN32K |
                         SYSCTRL_XOSC32K_XTALEN |
                         SYSCTRL_XOSC32K_STARTUP(6) | 
                         SYSCTRL_XOSC32K_ENABLE;
}

#endif