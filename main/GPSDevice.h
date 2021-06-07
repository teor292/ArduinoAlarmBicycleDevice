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

        bool SetModeSettings(UBX_CFG_PM2& settings);

        bool SetMode(GPS_DEVICE_WORK_MODE mode);


    protected:
        Stream& stream_;
        NonUbxCallback non_ubx_callback_;
        WaitCallback wait_callback_;

        void wait_(MillisReadDelay& millis);

        bool read_ack_(uint8_t clsID, uint8_t msgID);

        bool set_continous_mode_();
        bool set_off_mode_();
        bool set_ps_mode_();

        bool send_rxm_msg_(LP_MODE mode);

        template<typename T>
        bool send_message_(T& message)
        {
            MillisReadDelay millis(stream_, wait_callback_);

            //1. wake up
            stream_.write(0xFF); //send ignoring characted
            millis.start(500); //wait for 0.5 s
            wait_(millis);

            //2. Send message && read ACK
            return send_message_no_wait_(message);
        }

        template<typename T>
        bool send_message_no_wait_(T& message)
        {
            //Send message
            message.Send(stream_);

            //Read ACK
            return read_ack_(message.message.clss, message.message.id);
        }
        
};

#endif