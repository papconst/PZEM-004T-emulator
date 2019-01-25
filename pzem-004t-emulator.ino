#include <Ethernet.h>
IPAddress ip = IPAddress(0,0,0,0);
#define MSG_LEN 7
#define CMD_DELAY 1000 // You can set to 0 if you don't want to wait. Responce delay emulation.

uint32_t Uh, Ul, Ih, Il, P, E, Ia;// Values to send
bool rU = true, rI = true, rP = true, rE = true;// All values are random by default

void setup() 
{
  Serial.begin(9600);
}
bool checkCrc(const uint8_t data[MSG_LEN])
{
  uint16_t crc = 0;
  for(uint8_t i = 0; i < MSG_LEN - 1; i++)
    crc += data[i];
  crc = (uint8_t)(crc & 0xFF);
  if(crc == data[MSG_LEN - 1]) return true;
  else return false;  
}
uint8_t getCrc(const uint8_t data[MSG_LEN])
{
  uint16_t crc = 0;
  for(uint8_t i = 0; i < MSG_LEN - 1; i++)
    crc += data[i];
  return (uint8_t)(crc & 0xFF); 
}

bool checkAddress(const uint8_t data[MSG_LEN])
{
  if(ip == IPAddress(0,0,0,0))// if address wasn't set...
  {
    if(data[0] == 0xB4)// ...and we received command to set new...
    {
      setAddress(data);// ...set & return true
      return true;
    }
    else return false;// Addr is not set & we'r not seting it now - ignoring command
  }
  if( ip == IPAddress(data[1],data[2],data[3],data[4]))// if addr is ...
    return true;// ... ours - we will work
  else 
    return false;// ... not ours - ignoring command
}
void sendData(const uint8_t com, const uint8_t arg)
{
  uint8_t command[MSG_LEN];
  command[0] = com;
  for(uint8_t i = 0; i < 4; i++) command[i+1] = ip[i];
  command[5] = arg;
  command[6] = getCrc(command);
  Serial.write(command, sizeof(command));  
  delay(1000);
}

void respV()
{//B0C0A80101001A
  if(rU)
  {
    // 210,0 .. 230,99 V
    Uh = random(210,231);
    Ul = random(0,100);
  }
  uint8_t data[MSG_LEN];
  data[0] = 0xA0;
  data[1] = 0x00;
  data[2] = Uh;
  data[3] = Ul;
  data[4] = 0x00;
  data[5] = 0x00;
  data[6] = getCrc(data);
  delay(CMD_DELAY);
  Serial.write(data,sizeof(data));
}
void respI()
{//B1C0A80101001B
  if(rI)
  {
    // 0,0 .. 99,99 A
    Ih = random(0,100);
    Il = random(0,100);
  }
  uint8_t data[MSG_LEN];
  data[0] = 0xA1;
  data[1] = 0x00;
  data[2] = Ih;
  data[3] = Il;
  data[4] = 0x00;
  data[5] = 0x00;
  data[6] = getCrc(data);
  delay(CMD_DELAY);
  Serial.write(data,sizeof(data));
}
void respP()
{//B2C0A80101001C
  if(rP)
  {
    // 0 .. 22 kW
    P = random(0,22001);
  }
  uint8_t data[MSG_LEN];
  data[0] = 0xA2;
  data[1] = highByte(P);
  data[2] = lowByte(P);
  data[3] = 0x00;
  data[4] = 0x00;
  data[5] = 0x00;
  data[6] = getCrc(data);
  delay(CMD_DELAY);
  Serial.write(data,sizeof(data));
}
void respE()
{//B3C0A80101001D
  if(rE)
  {
    // 0 .. 99 kW*h
    E = random(0,100000);
  }
  uint8_t data[MSG_LEN];
  data[0] = 0xA3;
  data[1] = E >> 16;
  data[2] = E >> 8 ;
  data[3] = E;
  data[4] = 0x00;
  data[5] = 0x00;
  data[6] = getCrc(data);
  delay(CMD_DELAY);
  Serial.write(data,sizeof(data));
}
void setAddress(const uint8_t data[MSG_LEN])
{//B4C0A80101001E
  ip[0] = data[1];// setting our address to ip
  ip[1] = data[2];
  ip[2] = data[3];
  ip[3] = data[4];
  uint8_t resp[MSG_LEN] = { 0xA4, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA4 };
  delay(CMD_DELAY);
  Serial.write(resp,sizeof(resp));// and sending responce
}
void setAlarm(const uint8_t data[MSG_LEN])
{//B5C0A801011433
  Ia = data[5];
  uint8_t resp[MSG_LEN] = { 0xA5, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA5 };
  delay(CMD_DELAY);
  Serial.write(resp,sizeof(resp));// and sending responce
}


void setU(const uint8_t data[MSG_LEN])// not a real PZEM command. Use to debug only.
{//B6C0A80101DCFC, 220 (dec) = DC (hex)
  rU = false;
  Uh = data[5];
  Ul = 0;
}
void setI(const uint8_t data[MSG_LEN])// not a real PZEM command. Use to debug only.
{//B7C0A801011435, 20 (dec) = 14 (hex)
  rI = false;
  Ih = data[5];
  Il = 0;
}
void setP(const uint8_t data[MSG_LEN])// not a real PZEM command. Use to debug only.
{//B8C0A80101FF21, 255 (dec) = FF (hex)
  rP = false;
  P = data[5];
}
void setE(const uint8_t data[MSG_LEN])// not a real PZEM command. Use to debug only.
{//B9C0A801011437, 20 (dec) = 14 (hex)
  rE = false;
  E = data[5];
}

void process()
{
  uint8_t data[MSG_LEN];
  uint8_t i = 0;
  while (Serial.available() > 0)
  {
    uint8_t incomingByte = Serial.read();
    data[i++] = incomingByte;
    delay(20);
  }
  if(checkCrc(data) && checkAddress(data))
  {
    switch (data[0]) 
    {
      case 0xB0: respV();//B0C0A80101001A
        break; 
      case 0xB1: respI();//B1C0A80101001B
        break; 
      case 0xB2: respP();//B2C0A80101001C
        break; 
      case 0xB3: respE();//B3C0A80101001D
        break; 
      case 0xB4: setAddress(data);//B4C0A80101001E
        break; 
      case 0xB5: setAlarm(data);//B5C0A801011433
        break; 
      // Next commands are not used in a real PZEM, use to debug emulator only!
      // You are able to set desired value to any value within 0..255 (single byte)
      case 0xB6: setU(data);//B6C0A80101DCFC, 220 V (dec) = DC (hex)
        break; 
      case 0xB7: setI(data);//B7C0A801011435, 20 A (dec) = 14 (hex)
        break; 
      case 0xB8: setP(data);//B8C0A80101FF21, 255 W (dec) = FF (hex)
        break; 
      case 0xB9: setE(data);//B9C0A801011437, 20 W*h (dec) = 14 (hex)
      break; 
    }
  }

}
void loop()
{
  if(Serial.available()) 
  {
    process();
  }
  delay(50);
}
