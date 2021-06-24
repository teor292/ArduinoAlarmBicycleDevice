#pragma once

#include "gps_header.h"

#if defined(GPS)


#include "TinyGPS++.h"
#include <Stream.h>
#include "header.h"
#include "GPSManualPSM.h"
#include "GPSAutoStater.h"
#include "sms.h"
#include "GPSCommands.h"
#include "GPSDataGetter.h"
#include "GPSSmsSender.h"
#include <Array.h>
#include "GPSTimeWrapperSender.h"
#include "GPSTimeSmsSendManager.h"
#include "vibro_header.h"

class GPSWorker : public AbstractFixCallable, public AbstractNonUBXCallable, public AbstractGPSGetterCallable
{
    public:

        explicit GPSWorker(Stream& gps_stream, Sms& sms, WaitCallback wait_callback,
            VibroAlarmChangeCallback alarm_change_callback);

        bool IsAlarmEnabled() const;

        void Read();

        void Work(bool was_alarm);

        void PerformCommand(const GPSCommandData& command);

        bool IsValidGPS(uint32_t valid_period_time) override;

        void NonUBXSymbol(uint8_t c) override;

        bool CheckAge(uint32_t valid_time) override;
        void Send(const Phone& phone, bool valid = true) override;

    protected:

        Stream& gps_stream_;
        Sms sms_;
        GPSAutoStater auto_stater_;
        GPSManualPSM manual_psm_;
        TinyGPSPlus gps_;
        GPSDataGetter data_getter_;
        GPSSettings settings_;
        GPSTimeSmsSendManager sms_send_manager_;
        VibroAlarmChangeCallback alarm_change_callback_;

        void send_ok_(const GPSCommandData& command);
        void send_error_(const GPSCommandData& command, GPS_ERROR_CODES code);

        void get_gps_(const GPSCommandData& command);
        void get_gps_reset_(const GPSCommandData& command);
        void get_last_gps_(const GPSCommandData& command);
        void set_gps_fix_(const GPSCommandData& command);
        void get_gps_fix_(const GPSCommandData& command);
        void set_gps_vibro_(const GPSCommandData& command);
        void get_gps_vibro_(const GPSCommandData& command);
        void set_gps_send_sms_(const GPSCommandData& command);
        void get_gps_send_sms_(const GPSCommandData& command);
        void set_gps_send_sms_remove_(const GPSCommandData& command);
        void gps_reset_settings_(const GPSCommandData& command);
        void gps_reset_device_(const GPSCommandData& command);
};


#endif