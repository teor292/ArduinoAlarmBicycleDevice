#include "GPSTimeSmsSendManager.h"

#if defined(GPS)


GPSTimeSmsSendManager::GPSTimeSmsSendManager(Sms& sms, TinyGPSPlus& gps)
 : sms_(sms),
    gps_(gps)
{}

void GPSTimeSmsSendManager::AddOrUpdateSender(const SendSettings& settings)
{
    auto index = find_sender_(settings);

    if (-1 == index)
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
        senders_[index]->SetSettings(settings);
    }
}

bool GPSTimeSmsSendManager::RemoveSender(const SendSettings& settings)
{
    auto index = find_sender_(settings);
    if (-1 == index) return false;
    senders_.remove(index);
    return true;
}

void GPSTimeSmsSendManager::Reset()
{
    senders_.clear();
}

void GPSTimeSmsSendManager::Work()
{
    for (auto& sender : senders_)
    {
        sender->Work();
    }
}

uint32_t GPSTimeSmsSendManager::GetNextDiffTime(uint32_t current_time)
{
    uint32_t min_diff_time = ULONG_MAX;
    for (auto &sender : senders_)
    {
        auto current_diff = sender->GetNextDiffTime(current_time);
        if (0 == current_diff) return 0;
        if (current_diff < min_diff_time)
        {
            min_diff_time = current_diff;
        }
    }
    return min_diff_time;
}

int GPSTimeSmsSendManager::find_sender_(const SendSettings& settings) const
{
    auto cnt = senders_.size();
    for ( int i = 0; i < cnt; ++i)
    {
        auto& sender = senders_[i];
        if ((*sender) == settings.send_data.values.phone)
        {
            return i;
        }
    }
    return -1;
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