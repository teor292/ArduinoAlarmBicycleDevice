#pragma once

#include <SoftwareSerial.h>
#include "SafeString.h"
#include "BlockTimeReader.h"

class BookReader
{
    public:

        explicit BookReader(SoftwareSerial& serial, BlockTimeReader &reader);

        void LoadAdminPhone(SafeString &buffer);

        void SetAdminPhone(const char* phone);

        const char* GetAdminPhone() const;

        bool IsEmpty() const;

    private:

        SoftwareSerial& serial_;
        BlockTimeReader& reader_;
        char admin_phone_number_[19]{0};
};