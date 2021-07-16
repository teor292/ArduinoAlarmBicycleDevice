#include "Caller.h"
#include "CLCCParser.h"
#include "scope_exit.h"

Caller::Caller(Stream& sim800, BlockTimeReader& reader, SafeString& buffer)
    : sim800_(sim800),
    reader_(reader),
    buffer_(buffer)
{}

void Caller::MakeCall(const char *phone)
{
    const static char ATH[] = "ATH";

    createSafeString(tmp, 20);
    if ('"' == phone[0])
    {
        tmp = &phone[1];
        tmp.removeLast(1);
    }
    else
    {
        tmp = phone;
    }

    sim800_.print(F("ATD"));
    sim800_.print(tmp.c_str());
    sim800_.println(';');
    if (!reader_.ReadStatusResponse(buffer_, 20000)) return;

    CLCC_CALL_STATE current_state = CLCC_CALL_STATE::UNKNOWN;

    auto& sim800 = sim800_;
    auto& reader = reader_;
    auto& buffer = buffer_;

    //exit from this function with ATH command and waiting response
    auto exit_lambda = [&sim800, &reader, &buffer]()
    {
        sim800.println(ATH);
        reader.ReadStatusResponse(buffer, 20000);
    };
    EXIT_SCOPE(exit_lambda);

    //must get DIALING state
    if (!wait_for_clcc_()) return;

    current_state = CLCCParser::Parse(buffer_);
    
    //if not -> smth wrong (can't call)
    if (CLCC_CALL_STATE::DIALING != current_state) return;

    //read for ALERTING state
    if (!wait_for_clcc_()) return;

    current_state = CLCCParser::Parse(buffer_);

    if (CLCC_CALL_STATE::ALERTING != current_state) return;

    //wait for response or hang or smth else for 1 min
    //do not check because it doesn't matter
    wait_for_clcc_(12);

    //ATH on exit
    return;
}

bool Caller::wait_for_clcc_(int attempts)
{
    const static char CLCC[] = "+CLCC:";
    //some count of attempts
    for (int i = 0; i < attempts; ++i)
    {
        if (reader_.ReadLine(buffer_, 5000))
        {
            if (buffer_.startsWith(CLCC)) return true;
            if (buffer_.startsWith("+CME ERROR")) return false;
            if (buffer_.startsWith("NO DIALTONE")) return false;
            if (buffer_.startsWith("BUSY")) return false;
            if (buffer_.startsWith("NO CARRIER")) return false;
            if (buffer_.startsWith("NO ANSWER")) return false;
        }
    }
    return false;
}