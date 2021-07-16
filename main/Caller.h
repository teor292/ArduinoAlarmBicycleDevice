#pragma once

#include "header.h"
#include "BlockTimeReader.h"
#include "MillisCallback.h"

class Caller
{
    public:

        explicit Caller(Stream& sim800, BlockTimeReader& reader, SafeString& buffer);

        void MakeCall(const char *phone);

    private:

        Stream& sim800_;
        BlockTimeReader& reader_;
        SafeString& buffer_;

        bool wait_for_clcc_(int attempts = 4);
};