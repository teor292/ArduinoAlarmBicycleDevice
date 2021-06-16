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
        text += gps.date.day();
        text += '.';
        text += gps.date.month();
        text += '.';
        text += gps.date.year();
        text += " ";
        if (gps.time.isValid())
        {
            text += gps.time.hour();
            text += ':';
            text += gps.time.minute();
            text += ':';
            text += gps.time.second();
        } else
        {   
            text += INVALID;
        }
        text += '\n';
        append_age_(gps.date.age());
    } else
    {
        text += INVALID;
    }

    text += F("Location: ");
    if (gps.location.isValid() && (0 == valid_age || (0 != valid_age && gps.location.age() < valid_age) ))
    {
        text += F("lat ");
        text += gps.location.lat();
        text += F("lon");
        text += gps.location.lng();
        text += '\n';
        append_age_(gps.location.age());
    } else
    {
        text += INVALID;
    }

    text += F("Altitude: ");
    if (gps.altitude.isValid() && (0 == valid_age || (0 != valid_age && gps.altitude.age() < valid_age)))
    {
        text += gps.altitude.meters();
        text += "m\n";
        append_age_(gps.altitude.age());
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
    text += time;
    text += ':';
    text += minutes;
    text += ':';
    text += seconds;
    text += '\n';
}

#endif