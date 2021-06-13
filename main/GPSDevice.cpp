#include "GPSDevice.h"

#if defined(GPS)

#define GPS_WAKE_TIME 5000

#include "header.h"

GPSDevice::GPSDevice(Stream& gps_stream, NonUbxCallback non_ubx_callback, WaitCallback wait_callback)
    : stream_(gps_stream),
    non_ubx_callback_(non_ubx_callback),
    wait_callback_(wait_callback)
{}

void GPSDevice::Initialize()
{
    if (initialized_) return;
    //don't know state of gps module. So, think it is in SLEEP,
    //need wake up and wait
    is_sleep_mode_ = true;
    wake_up_(GPS_WAKE_TIME);
    is_sleep_mode_ = GPS_OK(set_mode_(GPS_DEVICE_WORK_MODE::CONTINOUS));
    initialized_ = true;
}

GPS_ERROR_CODES GPSDevice::SetModeSettings(UBX_CFG_PM2& settings)
{
    Initialize();
    //1. Send message by default alg
    auto result = send_message_(settings);
    //currently do not save config:
    //for every new mode settings of mode must be set previously
    return result;
    //if (GPS_ERROR_CODES::OK != result) return result;
    
    //2. Write save settings message
    //UBX_CFG_CFG save_cfg;
   // save_cfg.message.saveMask.rxmConf = 1;
    //return send_message_no_wait_(save_cfg);
}

GPS_ERROR_CODES GPSDevice::SetRate(UBX_CFG_RATE& rate)
{
    Initialize();

    //1. Send message by default alg
    auto result = send_message_(rate);
    //if (!GPS_OK(result)) return result;
    return result;
    
    //2. Write save settings message
    //UBX_CFG_CFG save_cfg;
    //save_cfg.message.saveMask.navConf = 1;
    //return send_message_no_wait_(save_cfg);
}

GPS_ERROR_CODES GPSDevice::SetMode(GPS_DEVICE_WORK_MODE mode)
{
    Initialize();

    return set_mode_(mode);
}

GPS_ERROR_CODES GPSDevice::set_mode_(GPS_DEVICE_WORK_MODE mode)
{
    switch (mode)
    {
    case GPS_DEVICE_WORK_MODE::CONTINOUS:
        return set_continous_mode_();
    case GPS_DEVICE_WORK_MODE::PSMCT:
    case GPS_DEVICE_WORK_MODE::PSMOO:
        return set_ps_mode_();
    case GPS_DEVICE_WORK_MODE::OFF:
        return set_off_mode_();
    }
    return GPS_ERROR_CODES::NOT_SUPPORTED;
}

GPS_ERROR_CODES GPSDevice::ResetSettings()
{
    Initialize();
    UBX_CFG_CFG save_cfg;
    save_cfg.message.clearMask.navConf = 1;
    save_cfg.message.loadMask.navConf = 1;
    save_cfg.message.clearMask.rxmConf = 1;
    save_cfg.message.loadMask.rxmConf = 1;
    //device will be in continous mode after this
    auto result = send_message_(save_cfg);
    if (!GPS_OK(result)) return result;

    is_sleep_mode_ = false;

    return result;
}

void GPSDevice::ResetDevice()
{
    Initialize();
    UBX_CFG_RST rst;
    send_message_no_ack_(rst);
    millisDelay del;
    del.start(GPS_WAKE_TIME);
    wait_(del);

    //due to not save settings -> device must be in continous mode
    is_sleep_mode_ = false;
}

GPS_ERROR_CODES GPSDevice::set_continous_mode_()
{
    auto result = send_rxm_msg_(LP_MODE::CONTINOUS);
    //only if ok set is_sleep_mode_ to true
    is_sleep_mode_ = GPS_OK(result);
    return result;
}

GPS_ERROR_CODES GPSDevice::set_ps_mode_()
{
    //don't check result. 
    //If failed and set is_sleep_mode_ to true then everything will be ok,
    //but additional delay 500 ms
    is_sleep_mode_ = true;
    return send_rxm_msg_(LP_MODE::POWER_SAVING);
}

GPS_ERROR_CODES GPSDevice::set_off_mode_()
{
    UBX_RXM_PMREQ pmreq;
    pmreq.message.backup = 1;
    send_message_no_ack_(pmreq);
    is_sleep_mode_ = true;
    initialized_ = false;
    return GPS_ERROR_CODES::OK;
}

void GPSDevice::wake_up_(int timeout)
{
    if (!is_sleep_mode_) return;
    MillisReadDelay millis(stream_, wait_callback_);

    stream_.write(0xFF); //send ignoring characted
    millis.start(timeout); //wait for 0.5s (default timeout)
    wait_(millis);
}



GPS_ERROR_CODES GPSDevice::send_rxm_msg_(LP_MODE mode)
{
    UBX_CFG_RXM rxm;
    rxm.message.lpMode = mode;
    return send_message_(rxm, 2500);
}

GPS_ERROR_CODES GPSDevice::read_ack_(uint8_t clsID, uint8_t msgID, int timeout)
{
    UBX_ACK ack;
    auto result = ack.Read(stream_, non_ubx_callback_, timeout, wait_callback_);
    if (READ_UBX_RESULT::OK != result) 
    {
        PRINT("ACK failed: ");
        PRINTLN((int)result);
        return static_cast<GPS_ERROR_CODES>(result);
    }
    if (0 == ack.message.id)
    {
      PRINTLN("ack.message.id 0");
      return GPS_ERROR_CODES::UBX_NACK_MESSAGE;
    }

    if (ack.message.clsID != clsID
        || ack.message.msgID != msgID)
    {
        PRINTLN("clss id");
        PRINTLN(ack.message.clss);
        PRINTLN(clsID);
        PRINTLN(ack.message.id);
        PRINTLN(msgID);
        return GPS_ERROR_CODES::UBX_OTHER_MESSAGE;
    }

    return GPS_ERROR_CODES::OK;
}

void GPSDevice::wait_(millisDelay& millis)
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