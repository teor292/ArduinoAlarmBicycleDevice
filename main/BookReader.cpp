#include "BookReader.h"

BookReader::BookReader(Stream& serial, BlockTimeReader& reader)
    : serial_(serial),
    reader_(reader)
{}

bool BookReader::LoadAdminPhone(SafeString &buffer)
{
    buffer.clear();
    //admin phone in 1 position of phone book on this device
    serial_.println("AT+CPBR=1");
    if (!reader_.ReadStatusResponse(buffer, 3000)) return false;
    
    int index = buffer.indexOf("+CPBR: ");
    if (-1 == index) return false;
    buffer.substring(buffer, index);
    
    index = -1;
    for ( int i = 0; i < 3; ++i)
    {
        index = buffer.indexOf(',', index + 1);
        if (-1 == index) return false;
    }

    int last_index = buffer.indexOf('"', index + 2);
    //7 == index diff of `,"admin"`
    if (last_index - index != 7) return false;
    
    createSafeString(tmp_admin_str, 5);
    buffer.substring(tmp_admin_str, index + 2, last_index);
    if (tmp_admin_str != "admin") return false;

    index = buffer.indexOf('\"');
    last_index = buffer.indexOf('\"', index + 1);
    if (-1 == last_index) return false;

    createSafeStringFromCharArray(result, admin_phone_number_);
    buffer.substring(result, index, last_index + 1);

    //read last OK
    reader_.ReadLine(buffer, 1000);

    return true;

}

void BookReader::SetAdminPhone(const char* phone)
{
    //AT+CPBW=20,"phone",145,"admin"
    //39 for max phone number + sizeof(ERROR) + CRLF
    createSafeString(tmp_str, 46);
    tmp_str = "AT+CPBW=1,";
    tmp_str += phone;
    tmp_str += ",145,\"admin\"";
    serial_.println(tmp_str);

    //read ok
    reader_.ReadStatusResponse(tmp_str, 3000);
    createSafeStringFromCharArray(result, admin_phone_number_);
    result = phone;
}

const char* BookReader::GetAdminPhone() const
{
    return admin_phone_number_;
}

bool BookReader::IsEmpty() const
{
    return admin_phone_number_[0] == 0;
}