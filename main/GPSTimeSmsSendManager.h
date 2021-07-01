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

        bool RemoveSender(const SendSettings& settings) override;

        void Reset() override;

        void Work() override;

        uint32_t GetNextDiffTime(uint32_t current_time);

    
    protected:

        Sms& sms_;
        TinyGPSPlus& gps_;
        using GPSTimeSmsSenderType = std::shared_ptr<GPSTimeSmsSender>;
        Array<GPSTimeSmsSenderType, MAX_SMS_SENDERS> senders_;

        void notice_replace_(const SendSettings& settings);
        int find_sender_(const SendSettings& settings) const;
};

#endif