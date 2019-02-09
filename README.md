# PZEM-004T-emulator
Arduino/ESP8266 sketch that emulates PZEM-004T. You may test your device if postal service is too slow ;)

Commands B0 .. B5 are native PZEM instructions.
By default, all 'measured' by emulator values (U,I,P,E) are random within working range (check PZEM datasheet). But you can set desired values with commands B6 .. B9. They are having same syntax as native, value you set is sixth bit (before checksum). Remebmer that real PZEM won't responce on those commands.

Sketch tested on Arduino Nano/Pro mini and ESP-12F with this lib https://github.com/papconst/pzem-004t-Arduino-ESP8266
Also remember about logic levels (5V for Arduino and 3.3 for ESP8266).
