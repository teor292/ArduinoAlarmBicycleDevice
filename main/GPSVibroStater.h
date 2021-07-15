#pragma once

#include "gps_define.h"

#if defined(GPS)

#include "VibroReader.h"

class GPSVibroStater : public AbstractVibroCallback
{
    public:

        void Alarm() override;
        void EnableAlarm(bool enable);
        bool IsAlarm();
    
    protected:
        bool enabled_{false};
        bool is_alarm_{false};
};


#endif