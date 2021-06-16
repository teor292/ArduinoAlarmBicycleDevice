#pragma once

#include "gps_header.h"

#if defined(GPS)

#include "GPSAbstractSender.h"
#include "sms.h"

class GPSSmsSender : public AbstractGPSSender
{
    public:
        explicit GPSSmsSender(Sms& sms);

        void SetDefaultData(const SenderData& data) override;

        void SendGPS(TinyGPSPlus& gps) override;

        void SendGPS(TinyGPSPlus& gps, const SenderData& data, uint32_t valid_age) override;

    protected:

        Sms& sms_;
        Phone phone_;

        void send_gps_(TinyGPSPlus& gps, const Phone& phone, uint32_t valid_age);
};

#endif