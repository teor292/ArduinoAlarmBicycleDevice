#include "GPSTimeSmsSendManager.h"

#if defined(GPS)


GPSTimeSmsSendManager::GPSTimeSmsSendManager(Sms& sms, TinyGPSPlus& gps)
 : sms_(sms),
    gps_(gps)
{}

void GPSTimeSmsSendManager::AddOrUpdateSender(const SendSettings& settings)
{
    GPSTimeSmsSenderType *founded = nullptr;
    for (auto& sender : senders_)
    {
        if ((*sender) == settings.send_data.values.phone)
        {
            founded = &sender;
            break;
        }
    }

    if (nullptr == founded)
    {
        //not found
        if (senders_.max_size() == senders_.size() )
        {
            notice_replace_(settings);
            senders_.remove(0);
        }
        GPSTimeSmsSenderType new_sender(new GPSTimeSmsSender(sms_, gps_));
        new_sender->SetSettings(settings);
        senders_.push_back(new_sender);
    } 
    else
    {
        (*founded)->SetSettings(settings);
    }
}

void GPSTimeSmsSendManager::Work()
{
    for (auto& sender : senders_)
    {
        sender->Work();
    }
}

void GPSTimeSmsSendManager::notice_replace_(const SendSettings& settings)
{
    sms_.SetPhone(senders_[0]->GetPhone().phone);
    createSafeString(tmp, PHONE_BUF_LENGTH + 20);
    tmp = F("You replaced by ");
    tmp += settings.send_data.values.phone.phone;
    sms_.SendSms(tmp.c_str());
}

#endif