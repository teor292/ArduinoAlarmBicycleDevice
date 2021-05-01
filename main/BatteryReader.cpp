#include "BatteryReader.h"

BatteryReader::BatteryReader(SafeString& tmp_string, SoftwareSerial& serial, BlockTimeReader &reader)
    : tmp_string_(tmp_string),
    serial_(serial),
    reader_(reader),
    digit_value_(0)
{}

bool BatteryReader::ReadBattery()
{
    serial_.println("AT+CBC");

    if (!reader_.ReadSomeResponse(tmp_string_, 1000)) return false;

    int index_start_percent = tmp_string_.indexOf(',') + 1;
    if (-1 == index_start_percent) return false;
    int index_end_percent = tmp_string_.indexOf(',', index_start_percent);
    if (-1 == index_end_percent) return false;
    int index_start_voltage = index_end_percent + 1;
    int index_end_voltage = tmp_string_.indexOf(13, index_start_voltage);
    if (-1 == index_end_voltage) return false;

    createSafeStringFromCharArray(result, battery_result_);
    result = "Bat: ";
    createSafeString(tmp, 4);
    tmp_string_.substring(tmp, index_start_percent, index_end_percent);
    int tmp_value = 0;
    tmp.toInt(tmp_value);
    digit_value_ = static_cast<unsigned char>(tmp_value);
    result += tmp;
    result += ' ';
    result += '%';
    result += '\n';
    tmp_string_.substring(tmp, index_start_voltage, index_end_voltage);
    result += tmp;
    result += ' ';
    result += 'm';
    result += 'v';

    return true;
}

const char *BatteryReader::GetData() const
{
    return battery_result_;
}

unsigned char BatteryReader::GetDigitValue() const
{
    return digit_value_;
}