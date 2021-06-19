#pragma once


#include "gps_header.h"

#if defined(GPS)

#include "header.h"
#include <Array.h>
#include "sms.h"
#include "TinyGPS++.h"
#include "Phone.h"

struct PhoneData
{
    Phone phone;
    uint32_t time{0};
};

class AbstractGPSGetterCallable
{
    public:
        virtual ~AbstractGPSGetterCallable(){}
        virtual bool CheckAge(uint32_t valid_time) = 0;
        virtual void Send(const Phone& phone, bool valid = true) = 0;
};

class GPSDataGetter
{
    public:
        explicit GPSDataGetter(AbstractGPSGetterCallable* callback);

        void AddToWait(const Phone& phone);

        void RemoveFromWait(const Phone& phone);

        bool IsActive() const;

        void Work(); 

    protected:

        AbstractGPSGetterCallable* callback_;

        static const int MAX_PHONES = 3;

        Array<PhoneData, MAX_PHONES> phones_;

        void remove_non_active_phones_();

        bool phone_active_(const PhoneData& phone, uint32_t time) const;
};


#endif