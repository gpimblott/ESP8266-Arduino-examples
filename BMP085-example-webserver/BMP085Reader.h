#ifndef bmp085reader_h
#define bmp085reader_h

#include <Arduino.h>
#include <stdlib.h>
#include <Wire.h>

#define BMP085_ADDRESS 0x77  // I2C address of BMP085

const unsigned char OSS = 0;  // Oversampling Setting

//
//   BMP085 code based on  code by: Jim Lindblom (SparkFun Electronics)
//   date: 18 Jan 2011
//   updated: 26 Feb 2013
//   updated: 27 Jan 2016 (G.Pimblott)
//   license: CC BY-SA v3.0 - http://creativecommons.org/licenses/by-sa/3.0/
//
//   Get pressure and temperature from the BMP085 and calculate
//   altitude. Serial.print it out at 9600 baud to serial monitor.
//
//   Updated to work with BMP085 using i2c on ESP8266 and converted to a reusable class
//   by G.Pimblott (27/1/2016)
//
class BMP085Reader
{
  public:
  
    void init(int sda, int scl);
    void readValues();

    double getTemperature();
    long getPressure();
    float getAltitude();

 private:
    // Last readings
    double temperature;
    long pressure;
    float altitude;

    // Use these for altitude conversions
    const float p0 = 101325;     // Standard pressure at sea level (Pa)
 
    
 private:
    short bmp085GetTemperature(unsigned int ut);
    long bmp085GetPressure(unsigned long up);

    void bmp085Calibration();
    char bmp085Read(unsigned char address);
    int bmp085ReadInt(unsigned char address);
    short bmp085ReadShort(unsigned char address);
    unsigned short bmp085ReadUShort(unsigned char address);
    unsigned int bmp085ReadUT();
    unsigned long bmp085ReadUP();
  
  private:
    // Calibration values
    short ac1;  
    short ac2;
    short ac3;
    unsigned short ac4;
    unsigned short ac5;
    unsigned short ac6;
    short b1;
    short b2;
    short mb;
    short mc;
    short md;

    // b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
    // so ...Temperature(...) must be called before ...Pressure(...).
    long b5;
};

#endif
