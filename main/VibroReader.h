#pragma once


#include "Array.h"
#include "AbstractNextAwakeTimeGetter.h"

class AbstractVibroCallback
{
    public:
        virtual ~AbstractVibroCallback(){}
        virtual void Alarm(bool alarm) = 0;
};

//inherit this class from AbstractNextAwakeTimeGetter for ControllerSleep logic:
//if alarm is enabled -> do not sleep
class VibroReader : public AbstractNextAwakeTimeGetter
{
    public:

        explicit VibroReader(int input);

        bool AddVibroCallback(AbstractVibroCallback* callback);

        //inc count of change by 1
        void ForceChange();
        //read pin and inc if changed
        void ReadChange();

        void SetCountChanges(int count_changes_per_second);

        int GetCountChanges()
        {
            return count_changes_per_second_;
        } 

        void EnableAlarm(bool enable);

        uint32_t NextNeccessaryDiffTime(uint32_t current_time) override;

    private:

        const int VIBRO_INPUT;
        int count_changes_per_second_{0};
        int current_count_changes_{0};
        unsigned long last_millis_{0};
        unsigned char previous_state_{0};
        unsigned char current_state_{0};
        bool enabled_{true};

        Array<AbstractVibroCallback*, 2> callbacks_;
};