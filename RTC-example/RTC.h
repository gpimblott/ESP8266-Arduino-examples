#ifndef RTC_H
#define RTC_H

#include <stdlib.h>
#include <Arduino.h>
#include <Wire.h>

#define DS3231_I2C_ADDRESS 0x68

class RTC {

  public:

    enum Days_of_Week {
      Sunday = 1, Monday = 2, Tuesday = 3, Wednesday = 4, Thursday = 5, Friday = 6, Saturday = 7
    };

    enum Months_of_Year {
      January = 1, February = 2, March = 3, April = 4, May = 5, June = 6,
      July = 7, August = 8, September = 9, October = 10, November = 11, December = 12
    };
    
    void init(int sda, int scl);

    String getTime();

    void setTime(  byte second,
                   byte minute,
                   byte hour,
                   Days_of_Week dayOfWeek,
                   byte dayOfMonth,
                   Months_of_Year month,
                   byte year);

    void readTime(byte *second,
                  byte *minute,
                  byte *hour,
                  byte *dayOfWeek,
                  byte *dayOfMonth,
                  byte *month,
                  byte *year);


  private:

    byte decToBcd(byte val);
    byte bcdToDec(byte val);
    String getDay(Days_of_Week d); 
};

#endif

