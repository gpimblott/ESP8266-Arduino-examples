# ESP8266-Arduino IDE

Example projects for Arduino IDE to be used with the ESP8266 System On a Chip 

The ESP8266 is an amazing little module, System On a Chip (SOC).  I have been using Arduinos in projects for a number of years, some of which have included network interfaces to connect them to the big bad internet.  The great thing about the ESP8266 is that is primarily a WiFi Module which just happens to have a powerful, comparably, processor and can be connected to sensors and other applications through its GPIOs.  

You can get these modules in a number of flavours including tiny production boards and larger dev boards (NodeMCU) with Lua installed and GPIOs broken out.

After trying Lua and NodeMCU I decided that the Arduino IDE was the way to go :)  So here are my projects to get various sensors and examples working on the ESP8266.

All are compiled using Arduino IDE 1.6.7 with the custom ESP8266 board installed and deployed to a NodeMCU dev board.



##Projects
**BMP085**
* BMP085-example-webserver
  *  Connects to WLAN and starts a webserver on port 80.  
  *  Calls to root URL responds with values from connected BMP085 sensor.

**GPS Example**
* GPS-example
  *  Connects to WLAN and start a webserver on port 80
  *  Connects to GPS using a SoftwareSerial port
  *  Outputs GPS position to serial console and webserver
  *  Calls to root URL reposond with current position etc

**Real Time Clock Example**
* RTC-example
  * Connects to WLAN and start a webserver on port 80
  * Connects to RTC using I2C
  * Respond to queries on the root path with the current time
  * Set the time using URL parameters to /set?hour=00&minute=00&second=00&dayofweek=xxxx&monthofyear=xxx&year=16
  * Example : http://address/set?hour=21&minute=21&monthofyear=jan&dayofweek=saturday&dayofmonth=30&year=16
  
