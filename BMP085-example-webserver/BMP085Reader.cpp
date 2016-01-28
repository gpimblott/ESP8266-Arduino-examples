
#include "BMP085Reader.h"

//
//   BMP085 code based on code by: Jim Lindblom (SparkFun Electronics)
//   date: 18 Jan 2011
//   updated: 26 Feb 2013
//   updated: 27 Jan 2016 (G.Pimblott)
//   license: CC BY-SA v3.0 - http://creativecommons.org/licenses/by-sa/3.0/
//
//   Get pressure and temperature from the BMP085 and calculate
//   altitude.
//
//   Updated to work with BMP085 using i2c on ESP8266 and converted to a reusable class
//   by G.Pimblott (27/1/2016)
//

// Setup communication with the sensor
void BMP085Reader::init(int sda,int scl)
{
  Serial.println("BMP085 Initialisation");
 
  // Setup the SDA(GPIO4) and SCL(GPIO5) - These can be any GPIO on the ESP8266
  Wire.begin(sda, scl);
  
  bmp085Calibration();
}


// Read the latest values and store in the public variables
void BMP085Reader::readValues()
{
  temperature = bmp085GetTemperature(bmp085ReadUT())* 0.1;
  pressure = bmp085GetPressure(bmp085ReadUP());
  altitude = (float)44330 * (1 - pow(((float) pressure / p0), 0.190295));
}

double BMP085Reader::getTemperature() 
{ 
  return temperature;
};


long BMP085Reader::getPressure() 
{ 
  return pressure;
};

float BMP085Reader::getAltitude() 
{ 
  return altitude;
};


//   Stores all of the bmp085's calibration values into global variables
//   Calibration values are required to calculate temp and pressure
//   This function should be called at the beginning of the program
void BMP085Reader::bmp085Calibration()
{
  ac1 = bmp085ReadShort(0xAA);
  ac2 = bmp085ReadShort(0xAC);
  ac3 = bmp085ReadShort(0xAE);
  ac4 = bmp085ReadUShort(0xB0);
  ac5 = bmp085ReadUShort(0xB2);
  ac6 = bmp085ReadUShort(0xB4);
  b1 = bmp085ReadShort(0xB6);
  b2 = bmp085ReadShort(0xB8);
  mb = bmp085ReadShort(0xBA);
  mc = bmp085ReadShort(0xBC);
  md = bmp085ReadShort(0xBE);
}

// Calculate temperature given the uncalibrated temperature(ut)
// Value returned will be in units of 0.1 deg C
short BMP085Reader::bmp085GetTemperature(unsigned int ut)
{
  long x1, x2;

  x1 = (((long)ut - (long)ac6) * (long)ac5) >> 15;
  x2 = ((long)mc << 11) / (x1 + md);
  b5 = x1 + x2;
  return ((b5 + 8) >> 4);
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
long BMP085Reader::bmp085GetPressure(unsigned long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;

  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6) >> 12) >> 11;
  x2 = (ac2 * b6) >> 11;
  x3 = x1 + x2;
  b3 = (((((long)ac1) * 4 + x3) << OSS) + 2) >> 2;

  // Calculate B4
  x1 = (ac3 * b6) >> 13;
  x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (ac4 * (unsigned long)(x3 + 32768)) >> 15;

  b7 = ((unsigned long)(up - b3) * (50000 >> OSS));
  if (b7 < 0x80000000)
    p = (b7 << 1) / b4;
  else
    p = (b7 / b4) << 1;

  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  p += (x1 + x2 + 3791) >> 4;

  return p;
}

// Read 1 byte from the BMP085 at 'address'
char BMP085Reader::bmp085Read(unsigned char address)
{
  unsigned char data;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 1);
  while (!Wire.available())
    ;

  return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int BMP085Reader::bmp085ReadInt(unsigned char address)
{
  unsigned char msb, lsb;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 2);
  while (Wire.available() < 2)
    ;
  msb = Wire.read();
  lsb = Wire.read();

  return (int) msb << 8 | lsb;
}

// Read a signed short
short BMP085Reader::bmp085ReadShort(unsigned char address)
{
  return (short)bmp085ReadInt(address);
}

// Read an unsigned short
unsigned short BMP085Reader::bmp085ReadUShort(unsigned char address)
{
  return (unsigned short)bmp085ReadInt(address);
}

// Read the uncompensated temperature value
unsigned int BMP085Reader::bmp085ReadUT()
{
  unsigned int ut;

  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();

  // Wait at least 4.5ms
  delay(5);

  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

// Read the uncompensated pressure value
unsigned long BMP085Reader::bmp085ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;

  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS << 6));
  Wire.endTransmission();

  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3 << OSS));

  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);

  // Wait for data to become available
  while (Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();

  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8 - OSS);

  return up;
}
