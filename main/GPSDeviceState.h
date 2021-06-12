#pragma once

#include "gps_header.h"

#if defined(GPS)

class GPSDeviceBaseState
{
    public:

        explicit GPSDeviceBaseState(GPS_DEVICE_WORK_MODE mode);

        virtual ~GPSDeviceBaseState();

        virtual bool IsActive();

        virtual void Active(bool active);

        virtual void ForceResetActive();

        GPS_DEVICE_WORK_MODE GetMode() const;

        void SetMode(GPS_DEVICE_WORK_MODE mode);

    protected:

        GPS_DEVICE_WORK_MODE mode_;
};

class GPSDeviceState : public GPSDeviceBaseState
{
    public:

        explicit GPSDeviceState(GPS_DEVICE_WORK_MODE mode, unsigned long duration);
    
        bool IsActive() override;

        void Active(bool active) override;

        void ForceResetActive() override;
    protected:
        const unsigned long duration_{0};
        unsigned long last_alarm_time_{0};
};

#endif