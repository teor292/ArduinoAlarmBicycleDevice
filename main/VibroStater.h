#pragma once

#include "VibroReader.h"
#include "vibro_header.h"

class VibroStater
{
    public:

        explicit VibroStater(VibroReader &reader, VibroAlarmChangeCallback callback);

        bool Update();

        void EnableAlarm(bool enable);

        bool IsAlarmEnabled() const
        {
            return enabled_;
        }

    private:

        VibroReader &reader_;
        unsigned long last_alarm_time_{0};
        bool enabled_{false};
        VibroAlarmChangeCallback alarm_callback_;
};