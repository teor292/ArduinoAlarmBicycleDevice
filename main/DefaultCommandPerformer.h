#pragma once


#include "DefaultCommands.h"
#include "sms.h"
#include "BatteryReader.h"
#include "ModeSerial.h"
#include "BlockTimeReader.h"
#include "BookReader.h"
#include "Settings.h"
#include "VibroStater.h"

class DefaultCommandPerformer
{
    public:

        explicit DefaultCommandPerformer(Sms& sms, 
            BatteryReader& battery, 
            ModeSerial& sim800,
            BlockTimeReader& reader,
            BookReader& book_reader,
            bool& send_alarm_battery,
            Settings& settings,
            VibroStater& vibro,
            unsigned long& last_enter_sleep_time);

        void PerformCommand(const DefaultCommandData& cmd);

        static bool PerformSim800Command(ModeSerial& sim800, BlockTimeReader& reader, const char *cmd); 

    protected:

        Sms& sms_;
        BatteryReader& battery_;
        ModeSerial& sim800_;
        BlockTimeReader& reader_;
        BookReader& book_reader_;
        bool& send_alarm_battery_;
        Settings& settings_;
        VibroStater& vibro_;
        unsigned long& last_enter_sleep_time_;

        void get_battery_(const DefaultCommandData& cmd);
        void get_time_(const DefaultCommandData& cmd);
        void get_signal_(const DefaultCommandData& cmd);
        bool get_signal_strength_(SafeString& str);
        void set_admin_(const DefaultCommandData& cmd);
        void get_admin_(const DefaultCommandData& cmd);
        void set_low_battery_(const DefaultCommandData& cmd);
        void get_low_battery_(const DefaultCommandData& cmd);
        void set_alarm_(const DefaultCommandData& cmd);
        void set_alarm_and_sms_(bool alarm);
        void get_alarm_(const DefaultCommandData& cmd);
        void set_alarm_sensity_(const DefaultCommandData& cmd);
        void get_alarm_sensity_(const DefaultCommandData& cmd);
        void set_mode_(const DefaultCommandData& cmd);
        void get_mode_(const DefaultCommandData& cmd);

        static void add_zero_(SafeString& tmp, uint8_t value);
};