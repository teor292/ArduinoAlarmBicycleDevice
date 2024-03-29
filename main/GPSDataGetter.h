#pragma once


#include "gps_header.h"

#if defined(GPS)

#include "header.h"
#include <Array.h>
#include "Phone.h"

struct PhoneData
{
    explicit PhoneData() = default;
    explicit PhoneData(const Phone& phone, uint32_t time)
        : phone(phone),
         time(time)
    {}
    Phone phone;
    uint32_t time{0};
};

class AbstractGPSGetterCallable
{
    public:
        virtual ~AbstractGPSGetterCallable(){}
        virtual bool CheckAge(uint32_t valid_time_ms) = 0;
        virtual void Send(const Phone& phone, bool valid = true) = 0;
};

class GPSDataGetter
{
    public:
        explicit GPSDataGetter(AbstractGPSGetterCallable* callback);

        bool AddToWait(const Phone& phone);

        bool RemoveFromWait(const Phone& phone);

        bool IsActive() const;

        void Work(); 

        void Reset();

    protected:

        AbstractGPSGetterCallable* callback_;

        static const int MAX_PHONES = 3;

        Array<PhoneData, MAX_PHONES> phones_;

        void remove_non_active_phones_();

        bool phone_active_(const PhoneData& phone, uint32_t time) const;

        int find_phone_(const Phone& phone) const;

        bool exist_non_active_(uint32_t time) const;
};


#endif