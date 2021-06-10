#pragma once

#include "GPSDevice.h"

#if defined(GPS)

class GPSAutoStater
{
    public:
        explicit GPSAutoStater(Stream& gps_stream, NonUbxCallback non_ubx_callback, WaitCallback wait_callback);
      
        void Initialize(const GPSAllModeSettings& modes_settings);

        GPS_ERROR_CODES SetModesSettings(const GPSAllModeSettings& modes_settings);

        GPS_ERROR_CODES SetCurrentRegime(GPSRegimeSettings& settings);

        GPSRegimeSettings GetCurrentRegime();

        GPS_ERROR_CODES ResetSettings();

        void ResetDevice();

        void Work(bool alarm);
    
    protected:
        GPSDevice device_;

        bool initialized_{false};
        unsigned long last_alarm_time_{0};
        bool is_in_alarm_{false};

        GPSRegimeSettings current_settings_;
        GPSAllModeSettings modes_settings_;

        GPS_ERROR_CODES set_current_mode_settings_();
        GPS_ERROR_CODES set_mode_device_(GPS_DEVICE_WORK_MODE mode);
        GPS_ERROR_CODES set_continous_mode_();
        GPS_ERROR_CODES set_psmct_mode_();
        GPS_ERROR_CODES set_psmoo_mode_();
        GPS_ERROR_CODES set_off_mode_();

        void check_alarm_settings_(GPSRegimeSettings& settings);
        
};

#endif