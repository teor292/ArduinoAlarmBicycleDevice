#pragma once


#include "gps_header.h"

#if defined(GPS)

#include "ubx_messages.h"
#include "MillisCallback.h"


//class that response for set gps module settings and state
//it is not contains current state and behaves like module is always in
//sleep state
class GPSDevice
{
    public:

        explicit GPSDevice(Stream& gps_stream, NonUbxCallback non_ubx_callback, WaitCallback wait_callback);

        void Initialize();

        bool SetModeSettings(UBX_CFG_PM2& settings);

        bool SetRate(UBX_CFG_RATE& rate);

        bool SetMode(GPS_DEVICE_WORK_MODE mode);


    protected:
        Stream& stream_;
        NonUbxCallback non_ubx_callback_;
        WaitCallback wait_callback_;

        bool initialized_{false};

        static const int DEFAULT_TIMEOUT = 1200;

        void wait_(MillisReadDelay& millis);

        bool read_ack_(uint8_t clsID, uint8_t msgID, int timeout = DEFAULT_TIMEOUT);

        bool set_continous_mode_();
        bool set_off_mode_();
        bool set_ps_mode_();

        bool send_rxm_msg_(LP_MODE mode);

        void wake_up_(int timeout = 500);

        template<typename T>
        bool send_message_(T& message, int timeout = DEFAULT_TIMEOUT)
        {
            wake_up_();

            //Send message && read ACK
            return send_message_no_wait_(message, timeout);
        }

        template<typename T>
        bool send_message_no_wait_(T& message, int timeout = DEFAULT_TIMEOUT)
        {
            //Send message
            message.Send(stream_);

            //Read ACK
            return read_ack_(message.message.clss, message.message.id, timeout);
        }

        template<typename T>
        void send_message_no_ack_(T& message)
        {
            wake_up_();
            
            //Send message
            message.Send(stream_);
        }      
};

#endif