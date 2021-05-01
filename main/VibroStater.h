#pragma once


class VibroStater
{
    public:

        explicit VibroStater(int input);

        bool Update();

        void SetCountChanges(unsigned int count_changes_per_second);

        void EnableAlarm(unsigned char enable);

    private:

        const int VIBRO_INPUT;
        unsigned int count_changes_per_second_;
        unsigned int current_count_changes_;
        unsigned long last_millis_;
        unsigned long last_alarm_time_;
        unsigned char previous_state_;
        unsigned char current_state_;
        unsigned char enabled_;
};