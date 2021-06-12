#pragma once

#include "GPSDevice.h"

#if defined(GPS)

#include <Array.h>
#include "GPSDeviceState.h"
#include <ArxSmartPtr.h>

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

        void Force();

        void ResetForce();
    
    protected:
        GPSDevice device_;

        bool initialized_{false};

        GPSRegimeSettings current_settings_;
        GPSAllModeSettings modes_settings_;

        static const int MAX_STATE_COUNT = 3;
        Array<std::shared_ptr<GPSDeviceBaseState>,MAX_STATE_COUNT> states_;

        uint16_t current_rate_time_{0};

        GPS_ERROR_CODES set_mode_device_(GPS_DEVICE_WORK_MODE mode);
        GPS_ERROR_CODES set_continous_mode_();
        GPS_ERROR_CODES set_psmct_mode_();
        GPS_ERROR_CODES set_psmoo_mode_();
        GPS_ERROR_CODES set_off_mode_();

        GPS_ERROR_CODES set_rate_(uint16_t time);

        void check_settings_(GPSRegimeSettings& settings);

        bool check_equals_(GPS_DEVICE_WORK_MODE mode, const GPSAllModeSettings& settings) const;

        GPS_DEVICE_WORK_MODE get_current_mode_() const;

        void reset_stater_settings_();
        
};

#endif