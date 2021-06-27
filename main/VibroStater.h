#pragma once

#include "VibroReader.h"
#include "vibro_header.h"

class VibroStater : public AbstractVibroCallback
{
    public:

        explicit VibroStater(VibroAlarmChangeCallback callback);

        bool Update();

        void EnableAlarm(bool enable);

        bool IsAlarmEnabled() const
        {
            return enabled_;
        }

        void Alarm(bool alarm) override;

    private:

        unsigned long last_alarm_time_{0};
        bool enabled_{false};
        bool is_alarm_{false};
        VibroAlarmChangeCallback alarm_callback_;
};