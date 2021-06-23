#include "DefaultCommandPerformer.h"
#include "TextCommands.h"

namespace
{
    const char ERROR[] = "Error";
    const char OK[] = "OK";
    const char ON[] = "ON";
    const char OFF[] = "OFF";
    const char DEFAULT_MODE_COMMAND[] = "AT+CSCLK=0";
    const char SLEEP_MODE_COMMAND[] = "AT+CSCLK=2";
    const char SLEEP[] = "sleep";
    const char DEF[] = "def";
}

DefaultCommandPerformer::DefaultCommandPerformer(Sms& sms, 
    BatteryReader& battery, 
    ModeSerial& sim800,
    BlockTimeReader& reader,
    BookReader& book_reader,
    bool& send_alarm_battery,
    Settings& settings,
    VibroStater& vibro,
    unsigned long& last_enter_sleep_time)
    : sms_(sms),
    battery_(battery),
    sim800_(sim800),
    reader_(reader),
    book_reader_(book_reader),
    send_alarm_battery_(send_alarm_battery),
    settings_(settings),
    vibro_(vibro),
    last_enter_sleep_time_(last_enter_sleep_time)
{}

void DefaultCommandPerformer::PerformCommand(const DefaultCommandData& cmd)
{
    sms_.SetPhone(cmd.phone.phone);
    switch (cmd.cmd)
    {
    case DEFAULT_COMMANDS::GET_BATTERY:
        get_battery_(cmd);
        break;
    case DEFAULT_COMMANDS::GET_TIME:
        get_time_(cmd);
        break;
    case DEFAULT_COMMANDS::GET_SIGNAL:
        get_signal_(cmd);
        break;
    case DEFAULT_COMMANDS::SET_ADMIN:
        set_admin_(cmd);
        break;
    case DEFAULT_COMMANDS::GET_ADMIN:
        get_admin_(cmd);
        break;
    case DEFAULT_COMMANDS::SET_LOW_BATTERY:
        set_low_battery_(cmd);
        break;
    case DEFAULT_COMMANDS::GET_LOW_BATTERY:
        get_low_battery_(cmd);
        break;
    case DEFAULT_COMMANDS::SET_ALARM:
        set_alarm_(cmd);
        break;
    case DEFAULT_COMMANDS::GET_ALARM:
        get_alarm_(cmd);
        break;
    case DEFAULT_COMMANDS::SET_ALARM_SENSITY:
        set_alarm_sensity_(cmd);
        break;
    case DEFAULT_COMMANDS::GET_ALARM_SENSITY:
        get_alarm_sensity_(cmd);
        break;
    case DEFAULT_COMMANDS::SET_MODE:
        set_mode_(cmd);
        break;
    case DEFAULT_COMMANDS::GET_MODE:
        get_mode_(cmd);
        break;
    default:
        sms_.SendSms(UNKNOWN);
        break;
    }
}

void DefaultCommandPerformer::get_mode_(const DefaultCommandData& cmd)
{
    auto mode = sim800_.GetMode();
    if (WORK_MODE::SLEEP == mode)
    {
        sms_.SendSms(SLEEP);
    }
    else if (WORK_MODE::STANDART == mode)
    {
        sms_.SendSms(DEF);
    }
    else
    {
        createSafeString(tmp, 15);
        tmp += UNKNOWN;
        tmp += " mode";
        sms_.SendSms(tmp.c_str());
    }
}

void DefaultCommandPerformer::set_mode_(const DefaultCommandData& cmd)
{
    if (WORK_MODE::SLEEP == cmd.work_mode)
    {
        if (PerformSim800Command(sim800_, reader_, SLEEP_MODE_COMMAND))
        {
            vibro_.EnableAlarm(false); //disable alarm in sleep mode
            sim800_.SetMode(WORK_MODE::SLEEP);
            sms_.SendSms(OK);
            last_enter_sleep_time_ = 0;
            return;
        }
        sms_.SendSms(ERROR);
        return;
    }
    if (WORK_MODE::STANDART == cmd.work_mode)
    {
        if (PerformSim800Command(sim800_, reader_, DEFAULT_MODE_COMMAND))
        {
            sim800_.SetMode(WORK_MODE::STANDART);
            sms_.SendSms(OK);
            return;
        } 
        sms_.SendSms(ERROR);
        return;
    }

    sms_.SendSms(ERROR);

}

void DefaultCommandPerformer::get_alarm_sensity_(const DefaultCommandData& cmd)
{
    auto sensity = vibro_.GetCountChanges();
    createSafeString(tmp, 10);
    tmp += sensity;
    sms_.SendSms(tmp.c_str());
}

void DefaultCommandPerformer::set_alarm_sensity_(const DefaultCommandData& cmd)
{
    vibro_.SetCountChanges(static_cast<unsigned int>(cmd.sensity));
    sms_.SendSms(OK);
}

void DefaultCommandPerformer::get_alarm_(const DefaultCommandData& cmd)
{
    if (settings_.alarm)
    {
        sms_.SendSms(ON);
    } 
    else
    {
        sms_.SendSms(OFF);
    }
}

void DefaultCommandPerformer::set_alarm_and_sms_(bool alarm)
{
    settings_.alarm = alarm;
    settings_.Save();
    vibro_.EnableAlarm(alarm);
    sms_.SendSms(OK);
}

void DefaultCommandPerformer::set_alarm_(const DefaultCommandData& cmd)
{
    if (ALARM_STATE::ON == cmd.alarm_state)
    {
        //when alarm enable -> disable sleep mode
        bool ok = WORK_MODE::SLEEP != sim800_.GetMode();
        if (WORK_MODE::SLEEP == sim800_.GetMode())
        {
            if (PerformSim800Command(sim800_, reader_, DEFAULT_MODE_COMMAND))
            {
                PRINTLN(F("SMST"));
                sim800_.SetMode(WORK_MODE::STANDART);
                ok = true;
            }
        }
        if (ok)
        {
            set_alarm_and_sms_(true);
        } else
        {
            sms_.SendSms(ERROR);
        }


    }
    else
    {
        set_alarm_and_sms_(false);
    }
}

bool DefaultCommandPerformer::PerformSim800Command(ModeSerial& sim800, BlockTimeReader& reader,  const char *cmd)
{
    createSafeString(tmp, 100);
    sim800.println(cmd);
    //don't know what to do with status code
    if (!reader.ReadStatusResponse(tmp, 1000)) return false;
    return -1 != tmp.indexOf(OK);
}

void DefaultCommandPerformer::get_low_battery_(const DefaultCommandData& cmd)
{
    if (send_alarm_battery_)
    {
        sms_.SendSms(ON);
    } 
    else
    {
        sms_.SendSms(OFF);
    }
}

void DefaultCommandPerformer::set_low_battery_(const DefaultCommandData& cmd)
{
    if (LOW_BATTERY_STATE::ON == cmd.low_battery_state)
    {
        send_alarm_battery_ = true;
    } 
    else
    {
        send_alarm_battery_ = false;
    }
    sms_.SendSms(OK);
}

void DefaultCommandPerformer::get_admin_(const DefaultCommandData& cmd)
{
    if (book_reader_.IsEmpty())
    {
        sms_.SendSms("No admin");
    } 
    else
    {
        auto admin_phone = book_reader_.GetAdminPhone();
        sms_.SendSms(admin_phone);
    }

}

void DefaultCommandPerformer::set_admin_(const DefaultCommandData& cmd)
{
      book_reader_.SetAdminPhone(cmd.phone.phone);
      sms_.SendSms(OK);
}

bool DefaultCommandPerformer::get_signal_strength_(SafeString& str)
{
    sim800_.println(F("AT+CSQ"));

    createSafeString(tmp, 50);

    if (!reader_.ReadStatusResponse(tmp, 1000)) return false;

    auto index = tmp.indexOf("+CSQ:");
    if (-1 == index) return false;

    tmp.substring(str, index + 6, index + 8);

    str += F(" (0-31)");
    return true;

}

void DefaultCommandPerformer::get_signal_(const DefaultCommandData& cmd)
{
    createSafeString(signal_str, 10);
    if (get_signal_strength_(signal_str))
    {
        sms_.SendSms(signal_str.c_str());
    } else
    {
        sms_.SendSms(ERROR);
    }
}

void DefaultCommandPerformer::get_battery_(const DefaultCommandData& cmd)
{
    if (!battery_.ReadBattery())
    {
        sms_.SendSms(ERROR);
    } else
    {
        sms_.SendSms(battery_.GetData());
    }
}

void DefaultCommandPerformer::get_time_(const DefaultCommandData& cmd)
{
    createSafeString(tmp, 19);
    add_zero_(tmp, cmd.date_time.day);
    tmp += (int)cmd.date_time.day;
    tmp += '.';
    add_zero_(tmp, cmd.date_time.month);
    tmp += (int)cmd.date_time.month;
    tmp += '.';
    tmp += (int)(cmd.date_time.year + 2000);
    tmp += ' ';
    add_zero_(tmp, cmd.date_time.hours);
    tmp += (int)cmd.date_time.hours;
    tmp += ':';
    add_zero_(tmp, cmd.date_time.minutes);
    tmp += (int)cmd.date_time.minutes;
    tmp += ':';
    add_zero_(tmp, cmd.date_time.seconds);
    tmp += (int)cmd.date_time.seconds;

    sms_.SendSms(tmp.c_str());
}

void DefaultCommandPerformer::add_zero_(SafeString& tmp, uint8_t value)
{
    if (value < 10)
    {
        tmp += '0';
    }
}
