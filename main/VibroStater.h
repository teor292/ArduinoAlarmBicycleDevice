#pragma once


class VibroStater
{
    public:

        explicit VibroStater(int input);

        bool Update();

        void SetCountChanges(int count_changes_per_second);

        int GetCountChanges()
        {
            return count_changes_per_second_;
        } 

        void EnableAlarm(bool enable);

    private:

        const int VIBRO_INPUT;
        int count_changes_per_second_;
        int current_count_changes_;
        unsigned long last_millis_;
        unsigned long last_alarm_time_;
        unsigned char previous_state_;
        unsigned char current_state_;
        bool enabled_;
};