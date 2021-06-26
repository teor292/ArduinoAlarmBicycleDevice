#include "GPSTextCreator.h"

#if defined(GPS)

void GPSTextCreator::CreateGpsText(SafeString& text, TinyGPSPlus& gps, uint32_t valid_age)
{
    //TODO
    //1. Location -> reference to google maps
    //2. Age by date and not period
    const auto INVALID = F("invalid\n");
    text.clear();
    text += F("Date: ");
    if (gps.date.isValid() && (0 == valid_age || (0 != valid_age && gps.date.age() < valid_age) ))
    {
        add_value_(text, gps.date.day());
        text += '.';
        add_value_(text, gps.date.month());
        text += '.';
        add_value_(text, gps.date.year());
        text += " ";
        if (gps.time.isValid())
        {
            add_value_(text, gps.time.hour());
            text += ':';
            add_value_(text, gps.time.minute());
            text += ':';
            add_value_(text, gps.time.second());
        } else
        {   
            text += INVALID;
        }
        text += '\n';
        append_age_(text, gps.date.age());
    } else
    {
        text += INVALID;
    }

    text += F("Location: ");
    if (gps.location.isValid() && (0 == valid_age || (0 != valid_age && gps.location.age() < valid_age) ))
    {
        text += F("http://maps.google.com/maps?q=");
        createSafeString(tmp, 15);
        tmp.print(gps.location.lat(), 7);
        text += tmp;
        text += ",";
        tmp.clear();
        tmp.print(gps.location.lng(), 7);
        text += tmp;
        text += '\n';
        append_age_(text, gps.location.age());
    } else
    {
        text += INVALID;
    }

    text += F("Altitude: ");
    if (gps.altitude.isValid() && (0 == valid_age || (0 != valid_age && gps.altitude.age() < valid_age)))
    {
        text += gps.altitude.meters();
        text += "m\n";
        append_age_(text, gps.altitude.age());
    } else
    {
        text += INVALID;
    }

}

void GPSTextCreator::append_age_(SafeString& text, uint32_t time)
{
    time /= 1000;
    auto seconds = time % 60;
    time /= 60;
    auto minutes = time % 60;
    time /= 60;

    text += F("Age: ");
    add_value_(text, time);
    text += ':';
    add_value_(text, minutes);
    text += ':';
    add_value_(text, seconds);
    text += '\n';
}

void GPSTextCreator::add_value_(SafeString& text, uint32_t value)
{
    if (value < 10)
    {
        text += '0';
    }
    text += value;
}

#endif