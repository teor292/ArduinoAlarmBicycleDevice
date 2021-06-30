#include "ControllerSleeper.h"

#include "time_utils.h"
#include "sleep_utils.h"

Array<AbstractNextAwakeTimeGetter*, 3> ControllerSleeper::getters_;
WORK_MODE ControllerSleeper::mode_ = WORK_MODE::STANDART;

void ControllerSleeper::AddTimeGetter(AbstractNextAwakeTimeGetter* getter)
{
    getters_.push_back(getter);
}

bool ControllerSleeper::Sleep(BatteryChecker &checker)
{
    if (!IsSleepMode()) return false;
    auto current_time = time();
    uint32_t min_diff = ULONG_MAX;
    for (auto& getter : getters_)
    {
        auto some_diff = getter->NextNeccessaryDiffTime(current_time);
        if (0 == some_diff) return false; //0 => do not sleep
        if (ULONG_MAX == some_diff) continue; //ignore if max
        if (some_diff < min_diff)
        {
            min_diff = some_diff;
        }
    }

    //if next awake must be in less than 30 seconds -> do not sleep
    if (min_diff < s_to_time(30))
    {
        return false;
    }

    go_to_sleep(checker, current_time, min_diff - s_to_time(20));
    return true;
}


void ControllerSleeper::SetMode(WORK_MODE mode)
{
    mode_ = mode;
}

WORK_MODE ControllerSleeper::GetMode()
{
    return mode_;
}