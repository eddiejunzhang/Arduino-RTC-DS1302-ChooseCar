#include <Wire.h>
#include <DS1302.h>

DS1302 rtc(5, 6, 7); // RST, DAT, CLK
int Pin1Green = 13;  //针脚，第一辆车可以使用的指示灯。
int Pin1Red = 12;  //针脚，第一辆车不可使用的指示灯。
int Pin2Green = 9;
int Pin2Red = 8;

int sYear = 2018;  //开始执行限号的年、月、日
int sMonth = 4;
int sDay = 9;

int sMondayLimit = 4;  //第一周周一限号第一个数，第二数字为4+5=9

int Car1 = 6;  //第一辆车的车号尾号
int Car2 = 9;

int Ban1Weekday; //第一辆车限行的日，1=星期一。如果为0，则不存在此车。
int Ban2Weekday; //第二辆车限行的日，1=星期一。如果为0，则不存在此车。

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
  rtc.setDOW(MONDAY);
  rtc.setTime(23, 59, 30);
  //日期格式是 日月年
  rtc.setDate(8, 7, 2018);
  rtc.writeProtect(true);

  pinMode(Pin1Green, OUTPUT);
  pinMode(Pin1Red, OUTPUT);
  pinMode(Pin2Green, OUTPUT);
  pinMode(Pin2Red, OUTPUT);
}
void loop()
{
  int DiffDays = GetDays(sYear, sMonth, sDay, rtc.getTime().year, rtc.getTime().mon, rtc.getTime().date);
  Serial.print(DiffDays); Serial.print("    ");

  int Round = int(DiffDays / (7 * 13));
  Serial.print(Round); Serial.print("    ");

  int Plus = fmod(Round, 5);  //计算出当起始的限号，向前移动了多少次。
  Serial.println( Plus);

  int curMondayLimit = sMondayLimit - Plus; //计算出当前这一周，周一是哪个号限行

  Ban1Weekday = fmod(Car1 - curMondayLimit + 1, 5);
  Ban2Weekday = fmod(Car2 - curMondayLimit + 1, 5);

  Serial.println(Ban1Weekday);
  Serial.println(Ban2Weekday);

  //如果当前日等于第一辆车限制日，则变灯
  if (rtc.getTime().dow == Ban1Weekday ) {
    digitalWrite(Pin1Green, LOW);
    digitalWrite(Pin1Red, HIGH);
  }
  else  {
    digitalWrite(Pin1Green, HIGH);
    digitalWrite(Pin1Red, LOW);
  }

  if (rtc.getTime().dow == Ban2Weekday ) {
    digitalWrite(Pin2Green, LOW);
    digitalWrite(Pin2Red, HIGH);
  }
  else  {
    digitalWrite(Pin2Green, HIGH);
    digitalWrite(Pin2Red, LOW);
  }

  getdatetime();
  //停顿1个小时
  for (int i = 1; i < 3600; i++) {
    delay(1000);
  }

  digitalWrite(Pin1Green, LOW);
  digitalWrite(Pin1Red, LOW);
}
//获取2个日期之间相差的天数
int GetDays(int iYear1, int iMonth1, int iDay1, int iYear2, int iMonth2, int iDay2)   //1. 确保 日期1 < 日期2
{
  unsigned int iDate1 = iYear1 * 10000 + iMonth1 * 100 + iDay1;
  unsigned int iDate2 = iYear2 * 10000 + iMonth2 * 100 + iDay2;
  if (iDate1 > iDate2)
  {
    iYear1 = iYear1 + iYear2 - (iYear2 = iYear1);
    iMonth1 = iMonth1 + iMonth2 - (iMonth2 = iMonth1);
    iDay1 = iDay1 + iDay2 - (iDay2 = iDay1);
  }

  //2. 开始计算天数
  //计算 从 iYear1年1月1日 到 iYear2年1月1日前一天 之间的天数
  int iDays = 0;
  for (int i = iYear1; i < iYear2; i++)
  {
    iDays += (IsLeapYear(i) ? 366 : 365);
  }

  //减去iYear1年前iMonth1月的天数
  for (int i = 1; i < iMonth1; i++)
  {
    switch (i)
    {
      case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        iDays -= 31;
        break;
      case 4: case 6: case 9: case 11:
        iDays -= 30;
        break;
      case 2:
        iDays -= (IsLeapYear(iYear1) ? 29 : 28);
        break;
    }
  }
  //减去iYear1年iMonth1月前iDay1的天数
  iDays -= (iDay1 - 1);
  //加上iYear2年前iMonth2月的天数
  for (int i = 1; i < iMonth2; i++)
  {
    switch (i)
    {
      case 1: case 3: case 5: case 7: case 8: case 10: case 12:
        iDays += 31;
        break;
      case 4: case 6: case 9: case 11:
        iDays += 30;
        break;
      case 2:
        iDays += (IsLeapYear(iYear2) ? 29 : 28);
        break;
    }
  }
  //加上iYear2年iMonth2月前iDay2的天数
  iDays += (iDay2 - 1);
  return iDays;
}

//判断给定年是否闰年
bool IsLeapYear(int iYear)
{
  if (iYear % 100 == 0)
    return ((iYear % 400 == 0));
  else
    return ((iYear % 4 == 0));
}
