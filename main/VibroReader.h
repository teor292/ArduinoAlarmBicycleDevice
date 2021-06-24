#pragma once


class VibroReader
{
    public:

        explicit VibroReader(int input);

        //inc count of change by 1
        void ForceChange();
        //read pin and inc if changed
        void ReadChange();
        //is current in alarming
        bool IsAlarm();

        void SetCountChanges(int count_changes_per_second);

        int GetCountChanges()
        {
            return count_changes_per_second_;
        } 

        void EnableAlarm(bool enable);

    private:

        const int VIBRO_INPUT;
        int count_changes_per_second_{0};
        int current_count_changes_{0};
        unsigned long last_millis_{0};
        unsigned char previous_state_{0};
        unsigned char current_state_{0};
        bool enabled_{true};
};