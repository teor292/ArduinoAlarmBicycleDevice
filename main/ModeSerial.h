#pragma once

#include "work_mode.h"
#include <SoftwareSerial.h>

class ModeSerial : public SoftwareSerial
{
    public:

        using SoftwareSerial::SoftwareSerial;

        void SetMode(WORK_MODE mode);
        WORK_MODE GetMode();
        
        //this method is used for write smth
        virtual size_t write(const uint8_t *buffer, size_t size) override;
        virtual size_t write(uint8_t c) override;

    private:

        void awake_if_sleep_();
        
        WORK_MODE current_mode_{WORK_MODE::STANDART};
        unsigned long last_time_{0};
};