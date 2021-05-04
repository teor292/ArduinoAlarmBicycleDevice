#pragma once

#include "work_mode.h"
#include <SoftwareSerial.h>

class ModeSerial : public SoftwareSerial
{
    public:

        using SoftwareSerial::SoftwareSerial;

        void SetMode(WORK_MODE mode);
        
        //this method is used for write smth
        virtual size_t write(const uint8_t *buffer, size_t size) override;

    private:
        
        WORK_MODE current_mode_{WORK_MODE::STANDART};
        unsigned long last_time_{0};
};