#include <Wire.h>
#include <DS1302.h>

DS1302 rtc(5, 6, 7); // RST, DAT, CLK
int Pin1Green = 13;
int Pin1Red = 12;

int Ban1Weekday = 3;
int Ban2Weekday = 1;

void getdatetime()
{
  Serial.println(rtc.getDateStr(FORMAT_LONG, FORMAT_LITTLEENDIAN, '/'));
  Serial.print(rtc.getDOWStr());
  Serial.print("    ");
  Serial.println(rtc.getTimeStr());
}
void setup()
{
  Serial.begin(9600);
  // 设置时间后, 需要注释掉设置时间的代码,并重新烧录一次. 以免掉电重新执行setup中的时间设置函数.
  rtc.halt(false);
  rtc.writeProtect(false);
  rtc.setDOW(3);
  rtc.setTime(23, 59, 30);
  rtc.setDate(12, 06, 2018);
  rtc.writeProtect(true);

  pinMode(Pin1Green, OUTPUT);
}
void loop()
{
  if (rtc.getTime().dow == Ban1Weekday ) {
    digitalWrite(Pin1Green, LOW);
    digitalWrite(Pin1Red, HIGH);
  }
  else  {
    digitalWrite(Pin1Green, HIGH);
    digitalWrite(Pin1Red, LOW);
  }

  getdatetime();
  delay(5000);

  digitalWrite(Pin1Green, LOW);
  digitalWrite(Pin1Red, LOW);
}
