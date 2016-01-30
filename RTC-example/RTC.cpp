#include "RTC.h"

//
// Real Time Clock class tested with DS3231 breakout board
//

// Convert decimal to binary coded decimal
byte RTC::decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}

// Convert binary coded decimal to decimal numbers
byte RTC::bcdToDec(byte val)
{
  return ( (val / 16 * 10) + (val % 16) );
}

// Initalise the I2C communication by setting the GPIOs
// use for SDA and SCL
void RTC::init(int sda, int scl)
{
  Serial.println("RTC Initialisation");

  // Setup the SDA(GPIO4) and SCL(GPIO5) - These can be any GPIO on the ESP8266
  Wire.begin(sda, scl);
}

void RTC::setTime(  byte second,
                    byte minute,
                    byte hour,
                    Days_of_Week dayOfWeek,
                    byte dayOfMonth,
                    Months_of_Year month,
                    byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd((byte)dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd((byte)month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

void RTC::readTime(byte *second,
                   byte *minute,
                   byte *hour,
                   byte *dayOfWeek,
                   byte *dayOfMonth,
                   byte *month,
                   byte *year)
{

  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);

  // Read the data
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

String RTC::getTime()
{
  String timeStr = "";
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

  // Get the data
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
           &year);

  Days_of_Week day = static_cast<Days_of_Week>(dayOfWeek);

  // Build a display string
  timeStr += hour;
  timeStr += ":";
  timeStr += minute;
  timeStr += ":";
  timeStr += second;
  timeStr += " ";
  timeStr += getDay(day);
  timeStr += " ";
  timeStr += dayOfMonth;
  timeStr += "/";
  timeStr += month;
  timeStr += "/";
  timeStr += year;

  return timeStr;
}

String RTC::getDay(Days_of_Week d)
{
  switch (d) {
    case Monday :
      return "Monday";
      break;
    case Tuesday :
      return "Tuesday";
      break;
    case Wednesday :
      return "Wednesday";
      break;
    case Thursday :
      return "Thursday";
      break;
    case Friday :
      return "Friday";
      break;
    case Saturday :
      return "Saturday";
      break;
    case Sunday :
      return "Sunday";
      break;
  }

  return " ? ";
}



