# ArduinoAlarmBicycleDevice
Arduino bicycle alarm/gps device project.

Based on Arduino NANO/Seeeduino XIAO (for GPS), SIM800L module, some vibro sensor (SW-420, SW-520D etc) and if neccessary NEO-M8N GNSS module.

NOTE: It is neccessary a very sensitive vibro sensor for bike alarm. (According to tests, SW-420 is not suitable for such purposes).

Vibro sensor(s) must be connected to GND and vibro pin(s).

If you assemble the device from this components and use the code in the repository, you can get SMS/call (and/or GPS) alarms device on your bike or anywhere.

Main features:
1) Call/SMS alarm on vibration (It turns on immediately after the corresponding command);
2) Adjustable vibration sensitivity;
3) Automatically checks the remaining battery level and send correspond sms to admin phone;
4) Sleep mode for microcontroller and/or SIM800L;
5) Manual get battery level, signal level, GSM time;
6) Getting GPS position by sms command;
7) Fixing GPS position by time in automatic mode (NOT send, only fix (get) from sattelites);
8) Sending GPS position by time in automatic mode (currently only for 2 phone numbers but it can be changed in code);
9) Fixing GPS position on vibration.

TODO LIST:
1) Check pin code (add pin code?);
2) A-GPS;
3) Telegram bot for commands and gps data.


YOU CAN USE THIS CODE AS YOU WANT AT YOUR OWN RISK.
