#pragma once

#include <Stream.h>
#include "SafeString.h"
#include "BlockTimeReader.h"

class BookReader
{
    public:

        explicit BookReader(Stream& serial, BlockTimeReader &reader);

        bool LoadAdminPhone(SafeString &buffer);

        void SetAdminPhone(const char* phone);

        const char* GetAdminPhone() const;

        bool IsEmpty() const;

    private:

        Stream& serial_;
        BlockTimeReader& reader_;
        char admin_phone_number_[19]{0};
};