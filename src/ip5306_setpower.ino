#include "config.h"
//设置ip5306持续升压
uint8_t BatteryLevel;
bool setPowerBoostKeepOn(bool en)
{
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en)
  {
    Wire.write(0b110111); 
    // 0x37 = 0b110111 TCALL example
       
  /*
 [1]      Boost EN (default 1)            [EXM note: if 0 ESP32 will not boot from battery]
 [1]      Charger EN (1)                  [EXM note: did  not observe difference]
 [1]      Reserved (1)                    [EXM note: did  not observe difference]
 [1]      Insert load auto power EN (1)   [EXM note: did  not observe difference]
 [1]      BOOST output normally open ( 1) [EXM note: if 0 will shutdown on ESP32 sleep after 32s]
 [1]      Key off EN: (0)                 [EXM note: could not detect difference]
  */
  
  }
  else
  {
    // Wire.write(0x35); // x35 => Autoshutdown - Will kill power of ESP32 while sleeping!
    //HEX 35 = 0b110101
  }
  return Wire.endTransmission() == 0;
}
//检测电池电量等级
int8_t getBatteryLevel()
{
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(0x78);
  if (Wire.endTransmission(false) == 0 && Wire.requestFrom(0x75, 1))
  {
    switch (Wire.read() & 0xF0)
    {
    case 0xE0:
      return 25;
    case 0xC0:
      return 50;
    case 0x80:
      return 75;
    case 0x00:
      return 100;
    default:
      return 0;
    }
  }
  return -1;
}
//读取电池端实时电压
void getBatteryFromADC()
{
  bat_mv = 0;
  uint32_t oversample = 0;
  for (size_t i = 0; i < 100; i++)
  {
    oversample += (uint32_t)analogRead(ADC_BAT);
  }
  bat_mv = (int)oversample / 100;
  bat_mv = ((float)bat_mv / 4096) * 3600 * 2;

  Serial.print("Battery from ADC: ");
  Serial.print(bat_mv);
  Serial.println("mV");
}
void PowerManagment(uint32_t time_delay)
{
 // Wire.begin(SDA, SCL);
  delay(100);
  bool isOk = setPowerBoostKeepOn(true); // Disables auto-standby, to keep 5V high during ESP32 sleep after 32Sec

  // Set console baud rate
  Serial.println(F("Started"));
  Serial.println(String("IP5306 setPowerBoostKeepOn: ") + (isOk ? "OK" : "FAIL"));
  Serial.println(String("IP5306 Battery level:") + getBatteryLevel());
  getBatteryFromADC();
}

void fun_getBatteryLevel()//获取电量等级更改图标
{
    uint8_t oldBatteryLevel=0,i=0; 
    while(i<=3)  //滤波处理
    {
       BatteryLevel=getBatteryLevel(); //获取电压等级
       if(BatteryLevel==oldBatteryLevel) i++;
       else i=0;
       oldBatteryLevel=BatteryLevel;
    }
        i=0;
        switch (BatteryLevel)
        {
          case 0:   p1=F16x16_b0;
          break;
          case 25:  p1=F16x16_b20;
          break;
          case 75:  p1=F16x16_b60;
          break;
          case 100: p1=F16x16_b100;
          break;
        default:
          break;
        }
}