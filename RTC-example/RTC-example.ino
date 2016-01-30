#include <Arduino.h>
#include <stdlib.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "RTC.h"

//
//    Example Code for ESP8266 and Real Time Clock (Testing with DS3231 using I2C) using Arduino IDE
//    Updated by G.Pimblott
//    Created: 30 Jan 2016 (G.Pimblott)
//
//    1) Connects to a specified wireless network using DHCP (IP output to serial)
//    2) Starts a webserver on port 80
//    3) Responds to requests to the root URL with the current date and time
//    4) Date and time can be set using URL parameters
//
//    By default it expects the RTC to use sda(4) and scl(5) on an NodeMCU development board.
//

// Console output rate
#define SERIALBAUD  9600

// Default led is on GPIO2
const int led = 2;

// Update these with your network settings
#include "passwords.h"

// Create the webserver
ESP8266WebServer server(80);

// RTC component
RTC rtcClock;

// Display the time
void displayTime()
{
  String message = "The current time is ";
  message += rtcClock.getTime();
  server.send(200, "text/plain", message);

  Serial.println( message );
}

// Set the time and Date
void setTime()
{
  byte hour = 0, minute = 0, second = 0;
  byte dayOfMonth = 0, year=16;
  RTC::Days_of_Week dayOfWeek = RTC::Monday;
  RTC::Months_of_Year monthOfYear = RTC::January;

  String message = "";
  for (uint8_t i = 0; i < server.args(); i++) {
    String arg =  server.argName(i);
    String value = server.arg(i);

    if (arg == "dayofweek" ) {
      Serial.println("Checking day");
      if ( value == "monday" ) dayOfWeek = RTC::Monday;
      else if ( value == "tuesday" ) dayOfWeek = RTC::Tuesday;
      else if ( value == "wednesday" ) dayOfWeek = RTC::Wednesday;
      else if ( value == "thursday" ) dayOfWeek = RTC::Thursday;
      else if ( value == "friday" ) dayOfWeek = RTC::Friday;
      else if ( value == "saturday" ) dayOfWeek = RTC::Saturday;
      else if ( value == "sunday" ) dayOfWeek = RTC::Sunday;
    } else if (arg == "dayofmonth" ) {
      dayOfMonth = (byte)atoi( value.c_str() );
    } else if ( arg == "monthofyear" ) {
      Serial.println("Checking month");
      if ( value == "jan" ) monthOfYear = RTC::January;
      else if ( value == "feb" ) monthOfYear = RTC::February;
      else if ( value == "mar" ) monthOfYear = RTC::March;
      else if ( value == "apr" ) monthOfYear = RTC::April;
      else if ( value == "may" ) monthOfYear = RTC::May;
      else if ( value == "jun" ) monthOfYear = RTC::June;
      else if ( value == "jul" ) monthOfYear = RTC::July;
      else if ( value == "aug" ) monthOfYear = RTC::August;
      else if ( value == "sep" ) monthOfYear = RTC::September;
      else if ( value == "oct" ) monthOfYear = RTC::October;
      else if ( value == "nov" ) monthOfYear = RTC::November;
      else if ( value == "dec" ) monthOfYear = RTC::December;
    } else if ( value == "year" ) {
      year = (byte)atoi( value.c_str() );
    } else if (arg == "hour" ) {
      hour = (byte)atoi( value.c_str() );
    } else if (arg == "minute" ) {
      minute = (byte)atoi( value.c_str() );
    } else if (arg == "second" ) {
      second = (byte)atoi( value.c_str() );
    }


    message += " " + server.argName(i) + ":" + server.arg(i) + ":\n";
  }

  // Set the date to 1/1/16
  rtcClock.setTime( second, minute, hour,
                    dayOfWeek,
                    dayOfMonth,
                    monthOfYear,
                    year );


  server.send(200, "text/plain", message);

  Serial.println( rtcClock.getTime());
}

// Page not found message
void handleNotFound()
{
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

//
// Connect to the router with the specified ssid and password
//
void initWebServer( const char* ssid, const char * password)
{
  Serial.println(" Connecting to WiFi router");
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(SERIALBAUD);

  Serial.println("");
  Serial.println("Starting...");

  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  // Initialise the web server
  initWebServer(ssid , password);
  
  server.on("/set", setTime);
  server.on("/", displayTime);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("HTTP server started");

  // Initialise RTC clock on GPIO4 and GPIO5
  rtcClock.init(4, 5);

}

// Loop handling the http server requests
void loop()
{
  server.handleClient();
}
