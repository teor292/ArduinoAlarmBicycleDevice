#include "GPSDevice.h"

#if defined(GPS)

#include "header.h"
#include <Arduino.h>
#include "time_utils.h"

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
    wake_up_();
    is_sleep_mode_ = !GPS_OK(set_mode_(GPS_DEVICE_WORK_MODE::CONTINOUS));
    initialized_ = true;
}


GPS_ERROR_CODES GPSDevice::SetModeSettings(UBX_CFG_PM2& settings)
{
    PRINTLN("SetModeSettings");
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
    PRINTLN("SetRate");
    Initialize();
    
    //1. Send message by default alg
    auto result = send_message_(rate);
    if (GPS_OK(result))
    {
        current_rate_ = rate.message.measRate;
    }
    //if (!GPS_OK(result)) return result;
    return result;
    
    //2. Write save settings message
    //UBX_CFG_CFG save_cfg;
    //save_cfg.message.saveMask.navConf = 1;
    //return send_message_no_wait_(save_cfg);
}

GPS_ERROR_CODES GPSDevice::SetMode(GPS_DEVICE_WORK_MODE mode)
{
    PRINTLN("SetMode");
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

GPS_ERROR_CODES GPSDevice::ResetDevice()
{
    PRINTLN("REset device");
    bool was_initialized = initialized_;
    Initialize();
    //there are many message $GNTXT in NMEA output when reset and awake, must find at least one of it
    //BUT: don't know state of gps module
    //if sleep -> we must wait until all $GNTXT comes
    //so
    //1. if sleep -> go to continous mode && wait for 3 seconds (all $GNTXT comes)
    //2. if not in sleep but was not initialized -> wait for 3 seconds for all $GNTXT (was in OFF mode)
    //3. Run reset
    //4. wait for $GNTXT
    //5. if get -> ok, else -> run reset again
    millisDelay mil;
    if (is_sleep_mode_)
    {
        PRINTLN("CALL CONTINOUS");
        auto code = set_continous_mode_();
        PRINT("CONT RESULT: ");
        PRINTLN((int)code);
        mil.start(3000);
        wait_(mil);
    }
    else if (!was_initialized)
    {
        mil.start(3000);
        wait_(mil);
    }


    bool enabled = true;
    UBX_CFG_RST rst;
    createSafeString(tmp, 100);
    for (int i = 0; i < ATTEMPT_SEND_COUNT && enabled; ++i)
    {
        tmp.clear();
        send_message_no_ack_(rst);
        mil.start(1000); 
        //wait for symbol
        while (!mil.justFinished() && enabled)
        {
            while (stream_.available())
            {
                int value = stream_.read();
                tmp += static_cast<char>(value);
                if (nullptr != non_ubx_callback_)
                {
                    non_ubx_callback_->NonUBXSymbol(static_cast<uint8_t>(value));
                }
                if (-1 != tmp.indexOf("$GNTXT"))
                {
                    enabled = false;
                    break;
                }
            }
            if (nullptr != wait_callback_)
            {
                wait_callback_();
            }
        }
    }

    if (enabled) return GPS_ERROR_CODES::UBX_READ_TIMEOUT;

    //due to not save settings -> device must be in continous mode
    is_sleep_mode_ = false;

    return GPS_ERROR_CODES::OK;
}

GPS_ERROR_CODES GPSDevice::set_continous_mode_()
{
    PRINTLN("CONT_MODE");
    auto result = send_rxm_msg_(LP_MODE::CONTINOUS);
    //only if ok set is_sleep_mode_ to false
    is_sleep_mode_ = !GPS_OK(result);
    return result;
}

GPS_ERROR_CODES GPSDevice::set_ps_mode_()
{
    PRINTLN("PS_MODE");
    //don't check result. 
    //If failed and set is_sleep_mode_ to true then everything will be ok,
    //but additional delay 500 ms
    is_sleep_mode_ = true;
    return send_rxm_msg_(LP_MODE::POWER_SAVING);
}

GPS_ERROR_CODES GPSDevice::set_off_mode_()
{
    //The problem is that messages can be ignored when another protocol is running.
    //Therefore, it is necessary to wait depending on the rate time. 
    //However, if we have a rate of 9 seconds and there are 3 unsuccessful attempts, 
    //then we will wait at least 31.5 seconds. 
    //Therefore, we will set the rate to 1 second. Then we will wait a maximum of 9 seconds.
    if (current_rate_ != 1000)
    {
        UBX_CFG_RATE rate;
        rate.message.measRate = 1000;
        SetRate(rate); //not check result. If failed -> wait for previous rate time
    }


    UBX_RXM_PMREQ pmreq;
    pmreq.message.backup = 1;
    

    //UBX message ignores when NMEA message receive
    //1. clear buffer
    //2. wait for gps modules data in rate+500 ms time.
    //3. if there is no symbol -> ok, gps modules in sleep mode
    bool enabled = true;
    for (int i = 0; i < ATTEMPT_SEND_COUNT && enabled; ++i)
    {
        send_message_no_ack_(pmreq);

        millisDelay millis;
        millis.start(1000); 
        //clear current buffer
        wait_(millis);
        millis.start(current_rate_ + 500); 
        //wait for symbol
        while (!millis.justFinished())
        {
            if (stream_.available())
            {
                break;
            }
            if (nullptr != wait_callback_)
            {
                wait_callback_();
            }
        }
        enabled = 0 != stream_.available();


    }
    if (enabled)
    {
        //failed
        return GPS_ERROR_CODES::UBX_READ_TIMEOUT;
    }
    is_sleep_mode_ = true;
    initialized_ = false;
    return GPS_ERROR_CODES::OK;
}

void GPSDevice::wake_up_(int timeout)
{
    if (!is_sleep_mode_) return;
    //auto current_time = millis();
    auto current_time = time();
    if (current_time - last_wake_time_ < s_to_time(2)) return;

    PRINT("WAKEUP: ");
    PRINTLN(timeout);
    {
        millisDelay millis;

        stream_.write(static_cast<uint8_t>(0xFF)); //send ignoring characted
        millis.start(timeout); //wait for 0.5s (default timeout)
        wait_(millis);
    }

    //last_wake_time_ = millis();
    last_wake_time_ = time();
}



GPS_ERROR_CODES GPSDevice::send_rxm_msg_(LP_MODE mode)
{
    UBX_CFG_RXM rxm;
    rxm.message.lpMode = mode;
    return send_message_(rxm);
}

GPS_ERROR_CODES GPSDevice::read_ack_(uint8_t clsID, uint8_t msgID, int timeout)
{
    PRINT("read_ack_: ");
    PRINT((int)clsID);
    PRINT(' ');
    PRINTLN((int)msgID);
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
        //must read from gps, because
        //1. GPS module can be in non sleep and here we think -> it is in sleep mode
        //2. Serial buffer can fill if GPS is in non sleep mode
        while (stream_.available())
        {
            uint8_t c = static_cast<uint8_t>(stream_.read());
            if (nullptr != non_ubx_callback_)
            {
                non_ubx_callback_->NonUBXSymbol(c);
            }
        }
    }
}

#endif