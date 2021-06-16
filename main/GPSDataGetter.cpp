#include "GPSDataGetter.h"


#if defined(GPS)

#include <SafeString.h>

#define TIME_WAIT 600000UL
#define TIME_VALID 60000UL

GPSDataGetter::GPSDataGetter(AbstractGPSGetterCallable* callback)
    : callback_(callback)
{}

void GPSDataGetter::AddToWait(Phone& phone)
{
    PhoneData phone_data{phone, millis()};
    phones_.push_back(phone_data);
}

void GPSDataGetter::RemoveFromWait(Phone& phone)
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
            callback_->Send(p, false);
        }
    }
    phones_.clear();
    for (auto& p : active_phones)
    {
        phones_.push_back(p);
    }
}


#endif
