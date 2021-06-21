#include "GPSDataGetter.h"


#if defined(GPS)

#include <SafeString.h>

#define TIME_WAIT 600000UL
#define TIME_VALID 60000UL

GPSDataGetter::GPSDataGetter(AbstractGPSGetterCallable* callback)
    : callback_(callback)
{}

bool GPSDataGetter::AddToWait(const Phone& phone)
{
    auto index = find_phone_(phone);
    if (-1 == index)
    {
        if (phones_.max_size() == phones_.size()) return false;
        PhoneData phone_data{phone, millis()};
        phones_.push_back(phone_data);
        return true;
    }
    //update time
    phones_[index].time = millis();
    return true;

}

void GPSDataGetter::RemoveFromWait(const Phone& phone)
{
    uint16_t i = 0;
    for (; i < phones_.size(); ++i)
    {
        auto& d = phones_[i];
        if (d.phone == phone)
        {
            break;
        }
    }
    if (phones_.size() == i) return;

    phones_.remove(i);
}

bool GPSDataGetter::IsActive() const
{
    auto time = millis();
    for (auto& p : phones_)
    {
        if (phone_active_(p, time)) return true;
    }
    return false;
}

void GPSDataGetter::Work()
{
    if (!callback_->CheckAge(TIME_VALID))
    {
        remove_non_active_phones_();
        return;       
    }

    for (auto& p : phones_)
    {
        callback_->Send(p.phone);
    }

    phones_.clear();
}

void GPSDataGetter::Reset()
{
    phones_.clear();
}

bool GPSDataGetter::phone_active_(const PhoneData& phone, uint32_t time) const
{
    return time - phone.time < TIME_WAIT;
}

void GPSDataGetter::remove_non_active_phones_()
{
    if (phones_.empty()) return;
    Array<PhoneData, MAX_PHONES> active_phones;
    auto time = millis();
    for (auto& p : phones_)
    {
        if (phone_active_(p, time))
        {
            active_phones.push_back(p);
        } else
        {
            callback_->Send(p.phone, false);
        }
    }
    phones_.clear();
    for (auto& p : active_phones)
    {
        phones_.push_back(p);
    }
}

int GPSDataGetter::find_phone_(const Phone& phone) const
{
    auto cnt = phones_.size();
    for (int i = 0; i < cnt; ++i)
    {
        auto& ph = phones_[i];
        if (ph.phone == phone) return i;
    }
    return -1;
}

#endif
