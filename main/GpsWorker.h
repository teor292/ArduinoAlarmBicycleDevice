#pragma once

#include "gps_header.h"

#if defined(GPS)


#include "TinyGPS++.h"
#include <Stream.h>
#include "header.h"
#include "GPSManualPSM.h"
#include "GPSAutoStater.h"
#include "sms.h"
#include "GPSCommands.h"
#include "GPSDataGetter.h"
#include "GPSSmsSender.h"
#include <Array.h>

class GPSWorker : public AbstractFixCallable, public AbstractNonUBXCallable, public AbstractGPSGetterCallable
{
    public:

        explicit GPSWorker(Stream& gps_stream, Sms& sms);

        void Read();

        void Work(bool was_alarm);

        void PerformCommand(const GPSCommandData& command);

        bool IsValidGPS(uint32_t valid_period_time) override;

        void NonUBXSymbol(uint8_t c) override;

        bool CheckAge(uint32_t valid_time) override;
        void Send(const Phone& phone, bool valid) override;

    protected:

        Stream& gps_stream_;
        Sms& sms_;
        GPSAutoStater auto_stater_;
        GPSManualPSM manual_psm_;
        TinyGPSPlus gps_;
        GPSSmsSender sms_sender_;
        Array<AbstractGPSSender, 1> senders_;
};


#endif