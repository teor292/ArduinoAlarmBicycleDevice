#pragma once

#include "work_mode.h"

#include "serial_header.h"

class ModeSerial : public CUART
{
    public:

        explicit ModeSerial();

        void SetMode(WORK_MODE mode);
        WORK_MODE GetMode();
        bool IsSleepMode();

        void ResetTime();
        
        //this method is used for write smth
        virtual size_t write(const uint8_t *buffer, size_t size) override;
        virtual size_t write(uint8_t c) override;

    private:

        void awake_if_sleep_();
        
        WORK_MODE current_mode_{WORK_MODE::STANDART};
        unsigned long last_time_{0};
};