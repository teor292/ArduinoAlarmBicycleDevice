#pragma once


#include <Array.h>
#include "BatteryChecker.h"
#include "AbstractNextAwakeTimeGetter.h"
#include "work_mode.h"


class ControllerSleeper
{
    public:

        static void AddTimeGetter(AbstractNextAwakeTimeGetter* getter);
        static bool Sleep(BatteryChecker& checker);
        static void SetMode(WORK_MODE mode);
        static WORK_MODE GetMode();
        static bool IsSleepMode()
        {
            return WORK_MODE::SLEEP == mode_;
        }

    private:
        //1. Battery checker
        //2. GPSWorker (GPSManualPSM)
        //3. Vibro reader (enabled or disabled)
        static Array<AbstractNextAwakeTimeGetter*, 3> getters_;
        static WORK_MODE mode_;

};