#pragma once

#include "GPSDevice.h"

#if defined(GPS)

#include <Array.h>
#include "GPSDeviceState.h"
#include <ArxSmartPtr.h>

enum class GPS_STATER_FORCE
{
    BY_COMMAND,
    BY_SERVICE
};

class GPSAutoStater
{
    public:
        explicit GPSAutoStater(Stream& gps_stream, NonUbxCallback non_ubx_callback, WaitCallback wait_callback);
      
        void Initialize();

        GPS_ERROR_CODES SetSettings(const GPSStateSettings& settings);

        GPSStateSettings GetSettings() const;

        GPS_ERROR_CODES ResetSettings();

        GPS_ERROR_CODES ResetDevice();

        GPS_ERROR_CODES Work(bool alarm);

        GPS_ERROR_CODES Force(GPS_STATER_FORCE force);

        GPS_ERROR_CODES ResetForce(GPS_STATER_FORCE force);
    
    protected:
        GPSDevice device_;

        bool initialized_{false};

        static const int MAX_STATE_COUNT = 4;
        Array<std::shared_ptr<GPSDeviceBaseState>,MAX_STATE_COUNT> states_;

        GPS_ERROR_CODES set_mode_device_(const GPSDeviceStateSettings& mode);
        GPS_ERROR_CODES set_continous_mode_(uint32_t rate);
        GPS_ERROR_CODES set_psmct_mode_(const GPSDeviceStateSettings& mode);
        GPS_ERROR_CODES set_psmoo_mode_(const GPSDeviceStateSettings& mode);
        GPS_ERROR_CODES set_psm_mode_(const GPSDeviceStateSettings& mode);
        GPS_ERROR_CODES set_off_mode_();

        GPS_ERROR_CODES set_rate_(uint16_t time);

        uint16_t current_rate_time_{1000};
        GPSDeviceStateSettings last_state_;

        GPSDeviceStateSettings get_current_mode_() const;

        void reset_stater_settings_();

        static GPSDeviceStateSettings get_device_states_settings_(const GPSFixSettings& settings);
        static GPSDeviceStateSettings get_device_states_settings_(const GPSAlarmSettings& settings);

        static GPSDeviceStateForce* default_force_();
        static GPSDeviceState* default_alarm_();
        static GPSDeviceBaseState* default_standart_();
        
};

#endif