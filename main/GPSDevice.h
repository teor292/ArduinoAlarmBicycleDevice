#pragma once


#include "gps_header.h"

#if defined(GPS)

#include "ubx_messages.h"
#include "MillisCallback.h"
#include "gps_error_header.h"


//class that response for set gps module settings and state
//it is not contains current state and behaves like module is always in
//sleep state
class GPSDevice
{
    public:

        explicit GPSDevice(Stream& gps_stream, NonUbxCallback non_ubx_callback, WaitCallback wait_callback);

        void Initialize();

        GPS_ERROR_CODES SetModeSettings(UBX_CFG_PM2& settings);

        GPS_ERROR_CODES SetRate(UBX_CFG_RATE& rate);

        GPS_ERROR_CODES SetMode(GPS_DEVICE_WORK_MODE mode);

        GPS_ERROR_CODES ResetSettings();

        GPS_ERROR_CODES ResetDevice();


    protected:
        Stream& stream_;
        NonUbxCallback non_ubx_callback_;
        WaitCallback wait_callback_;

        bool initialized_{false};
        bool is_sleep_mode_{true};

        uint32_t last_wake_time_{0};
        //for off software mode
        uint16_t current_rate_{1000};

        static const int DEFAULT_TIMEOUT = 1200;
        static const int ATTEMPT_SEND_COUNT = 3;

        void wait_(millisDelay& millis);

        GPS_ERROR_CODES read_ack_(uint8_t clsID, uint8_t msgID, int timeout = DEFAULT_TIMEOUT);

        GPS_ERROR_CODES set_continous_mode_();
        GPS_ERROR_CODES set_off_mode_();
        GPS_ERROR_CODES set_ps_mode_();

        GPS_ERROR_CODES send_rxm_msg_(LP_MODE mode);

        void wake_up_(int timeout = 500);

        GPS_ERROR_CODES set_mode_(GPS_DEVICE_WORK_MODE mode);

        template<typename T>
        GPS_ERROR_CODES send_message_(T& message, int timeout = DEFAULT_TIMEOUT)
        {
            wake_up_();

            //Send message && read ACK
            return send_message_no_wait_(message, timeout);
        }

        template<typename T>
        GPS_ERROR_CODES send_message_no_wait_(T& message, int timeout = DEFAULT_TIMEOUT)
        {
            //when get gps data vit NMEA protocol
            //UBX protocol not working!
            //so when send message -> device ignores it, header not found, must resend
            GPS_ERROR_CODES code = GPS_ERROR_CODES::UBX_READ_TIMEOUT_HEADER;
            for ( int i = 0; i < ATTEMPT_SEND_COUNT 
                && (GPS_ERROR_CODES::UBX_READ_TIMEOUT_HEADER == code 
                    || GPS_ERROR_CODES::UBX_OTHER_MESSAGE == code); ++i)
            {
                //Send message
                message.Send(stream_);

                //Read ACK
                code = read_ack_(message.message.clss, message.message.id, timeout);
            }
            return code;

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