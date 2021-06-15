#pragma once

#include "gps_header.h"

#if defined(GPS)

class GPSDeviceStateSettings
{
    public:
        explicit GPSDeviceStateSettings(GPS_DEVICE_WORK_MODE mode, uint32_t time = 5,
             bool psmct_do_not_enter_off = false,
             uint32_t search_time = 300UL)
            : mode_(mode),
            update_time_(time),
            do_not_enter_off_(psmct_do_not_enter_off),
            search_time_(search_time)
        {}

        GPS_DEVICE_WORK_MODE GetMode() const
        {
            return mode_;
        }

        uint32_t GetTime() const
        {
            return update_time_;
        }

        uint32_t GetTimeMs() const
        {
            return update_time_ * 1000UL;
        }

        bool GetNotEnterOff() const
        {
            return do_not_enter_off_;
        }

        uint32_t GetSearchTime() const
        {
            return search_time_;
        }

        uint32_t GetSearchTimeMs() const
        {
            return GetSearchTime() * 1000UL;
        }

        bool operator > (const GPSDeviceStateSettings& settings) const
        {
            if (static_cast<int>(mode_) < static_cast<int>(settings.mode_)) return true;
            if (static_cast<int>(mode_) > static_cast<int>(settings.mode_)) return false;
            if (GPS_DEVICE_WORK_MODE::PSMCT == mode_)
            {
                if (do_not_enter_off_ && !settings.do_not_enter_off_) return true;
                return false;
            }
            return update_time_ < settings.update_time_;
        }

        bool operator == (const GPSDeviceStateSettings& settings) const
        {
            return mode_ == settings.mode_ 
                && update_time_ == settings.update_time_
                && do_not_enter_off_ == settings.do_not_enter_off_
                && search_time_ == settings.search_time_;

        }

        bool operator != (const GPSDeviceStateSettings& settings) const
        {
            return !((*this) == settings);
        }

    protected:

        GPS_DEVICE_WORK_MODE mode_;
        //seconds
        uint32_t update_time_;
        //There should be no ambiguous settings in the code.
        //see GPSFixSettings
        bool do_not_enter_off_;
        //device searchTime (see neo8 docs)
        uint32_t search_time_;
};

class GPSDeviceBaseState
{
    public:

        explicit GPSDeviceBaseState(const GPSDeviceStateSettings& mode);

        virtual ~GPSDeviceBaseState();

        virtual bool IsActive();

        virtual void Active(bool active);

        virtual void ForceResetActive();

        const GPSDeviceStateSettings& GetMode() const;

        void SetMode(const GPSDeviceStateSettings& mode);

    protected:

        GPSDeviceStateSettings mode_;
};

class GPSDeviceState : public GPSDeviceBaseState
{
    public:

        explicit GPSDeviceState(const GPSDeviceStateSettings& mode, uint32_t duration);
    
        bool IsActive() override;

        void Active(bool active) override;

        void ForceResetActive() override;

        void SetDuration(uint32_t duration);
    protected:
        uint32_t duration_{0};
        uint32_t last_alarm_time_{0};
};

class GPSDeviceStateForce : public GPSDeviceState
{
    public:
        using GPSDeviceState::GPSDeviceState;   

        void Active(bool active) override;
};

#endif