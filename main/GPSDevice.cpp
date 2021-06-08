#include "GPSDevice.h"

#if defined(GPS)


#include "header.h"

GPSDevice::GPSDevice(Stream& gps_stream, NonUbxCallback non_ubx_callback, WaitCallback wait_callback)
    : stream_(gps_stream),
    non_ubx_callback_(non_ubx_callback),
    wait_callback_(wait_callback)
{}

void GPSDevice::Initialize()
{
    if (initialized_) return;
    //don't know state of gps modele. So, think it is SLEEP,
    //need wake up and wait
    wake_up_(5000);
    initialized_ = true;
}

bool GPSDevice::SetModeSettings(UBX_CFG_PM2& settings)
{
    Initialize();
    //1. Send message by default alg
    if (!send_message_(settings)) return false;
    
    //2. Write save settings message
    UBX_CFG_CFG save_cfg;
    save_cfg.message.saveMask.rxmConf = 1;
    return send_message_no_wait_(save_cfg);
}

bool GPSDevice::SetRate(UBX_CFG_RATE& rate)
{
    Initialize();

    //1. Send message by default alg
    if (!send_message_(rate)) return false;
    
    //2. Write save settings message
    UBX_CFG_CFG save_cfg;
    save_cfg.message.saveMask.navConf = 1;
    return send_message_no_wait_(save_cfg);
}

bool GPSDevice::SetMode(GPS_DEVICE_WORK_MODE mode)
{
    Initialize();

    switch (mode)
    {
    case GPS_DEVICE_WORK_MODE::CONTINOUS:
        return set_continous_mode_();
    case GPS_DEVICE_WORK_MODE::PSMCT:
    case GPS_DEVICE_WORK_MODE::PSMOO:
        return set_ps_mode_();
    case GPS_DEVICE_WORK_MODE::SOFTWARE_OFF:
        return set_off_mode_();
    }
    return false;
}

bool GPSDevice::set_continous_mode_()
{
    return send_rxm_msg_(LP_MODE::CONTINOUS);
}

bool GPSDevice::set_ps_mode_()
{
    return send_rxm_msg_(LP_MODE::POWER_SAVING);
}

bool GPSDevice::set_off_mode_()
{
    UBX_RXM_PMREQ pmreq;
    pmreq.message.backup = 1;
    send_message_no_ack_(pmreq);
    initialized_ = false;
    return true;
}

void GPSDevice::wake_up_(int timeout)
{
    MillisReadDelay millis(stream_, wait_callback_);

    stream_.write(0xFF); //send ignoring characted
    millis.start(timeout); //wait for 0.5s (default timeout)
    wait_(millis);
}



bool GPSDevice::send_rxm_msg_(LP_MODE mode)
{
    UBX_CFG_RXM rxm;
    rxm.message.lpMode = mode;
    return send_message_(rxm, 2500);
}

bool GPSDevice::read_ack_(uint8_t clsID, uint8_t msgID, int timeout)
{
    UBX_ACK ack;
    auto result = ack.Read(stream_, non_ubx_callback_, timeout, wait_callback_);
    if (READ_UBX_RESULT::OK != result) 
    {
        PRINT("ACK failed: ");
        PRINTLN((int)result);
        return false;
    }
    if (0 == ack.message.id)
    {
      PRINTLN("ack.message.id 0");
      return false;
    }

    if (ack.message.clsID != clsID
        || ack.message.msgID != msgID)
    {
        PRINTLN("clss id");
        PRINTLN(ack.message.clss);
        PRINTLN(clsID);
        PRINTLN(ack.message.id);
        PRINTLN(msgID);
        return false;
    }

    return true;
}

void GPSDevice::wait_(MillisReadDelay& millis)
{
    while (!millis.justFinished())
    {
        if (nullptr != wait_callback_)
        {
            wait_callback_();
        }
    }
}

#endif