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

        bool operator == (const Phone& phone) const
        {
            return phone_ == phone;
        }

        const Phone& GetPhone() const
        {
            return phone_;
        }

        static void SendGPS(Sms& sms, TinyGPSPlus& gps, const SenderData& data, uint32_t valid_age);

    protected:

        Sms& sms_;
        Phone phone_;

        static void send_gps_(Sms& sms, TinyGPSPlus& gps, const Phone& phone, uint32_t valid_age);
};

#endif