#pragma once

#include "gps_header.h"

#if defined(GPS)

#include "sms.h"
#include <Array.h>
#include "GPSTimeSendAbstractManager.h"
#include "GPSTimeSmsSender.h"
#include <ArxSmartPtr.h>

class GPSTimeSmsSendManager : public AbstractGPSTimeSendManager
{
    public:

        explicit GPSTimeSmsSendManager(Sms& sms, TinyGPSPlus& gps);

        void AddOrUpdateSender(const SendSettings& settings) override;

        void Work() override;

    
    protected:

        Sms& sms_;
        TinyGPSPlus& gps_;
        using GPSTimeSmsSenderType = std::shared_ptr<GPSTimeSmsSender>;
        Array<GPSTimeSmsSenderType, MAX_SMS_SENDERS> senders_;

        void notice_replace_(const SendSettings& settings);
};

#endif