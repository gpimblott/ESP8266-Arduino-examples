#include <stdlib.h>

#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Ticker.h>

#include "TinyGPS++.h"

//
//    Example Code for ESP8266 and Bloc GPS6MV2 GPS board and the Arduino IDE
//    Updated by G.Pimblott
//    Created: 29 Jan 2016 (G.Pimblott)
//
//    Uses TinyGPS+ and SimpleTimer to output the current GPS position every second to the serial port
//    The GPS module is connected using SoftwareSerial to GPIO13(TX) and GPIO15(RX) - D7 and D8 
//    on the NodeMCU :)
//

const int led = 2;

// Update these with your network settings
#include "passwords.h"

// Set this value equal to the baud rate of your GPS
#define GPSBAUD  9600

// Console output rate
#define SERIALBAUD  115200

// Create the webserver
ESP8266WebServer server(80);

// Setup the software serial port
SoftwareSerial uart_gps(13, 15);

// Setup the GPS
TinyGPSPlus gps;

// the timer object
Ticker outputTicker;

boolean gpsValid = false;
char data[80] = "Unknown";

// Local functions
void outputData();


// Process a request for the data
void handleRoot() {
  digitalWrite(led, 1);
  String message = "Current position is ??\n\n";
  message += data;

  server.send(200, "text/plain", message);
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

//
// Initial setup routine
//
// 1) Open serial port for logging
// 2) Initialise the webserver
// 3) Initialise the serial to recieve the GPS feed
//
void setup()
{
  Serial.begin(SERIALBAUD);

  Serial.println();
  Serial.println(ESP.getResetInfo());
  Serial.println("");
  Serial.println("Starting...");

  // Set the output led to high it will go off when we have a lock 
  pinMode(led, OUTPUT);
  digitalWrite( led , HIGH );
  
  // Initialise the web server
  initWebServer(ssid , password);
  
  server.on("/", handleRoot);
  server.begin();

  //ESP.wdtDisable();
  
  Serial.println("HTTP server started");

  // Setup GPS
  Serial.println("Initialising GPS port");
  uart_gps.begin(GPSBAUD);

  // Set the tocker to output data every 2 second
  outputTicker.attach( 3.0 , outputData);
}


// Main loop
// 1) Read GPS data and process
// 2) Service HTTP requests
void loop()
{
  // Check for GPS data
  // A yield is included to allow background tasks to continue
  while (uart_gps.available() > 0) {
    //Serial.print( uart_gps.read() );
    gps.encode(uart_gps.read());
    ESP.wdtFeed();
    yield();
  }
  
  // service HTTP requests
  server.handleClient();
}

//
// Write the data to the SD card
//
void outputData(){

  // If the GPS location is invalid then keep the
  // GPS LED lit
  if( !gps.location.isValid() ) {
    digitalWrite( led , HIGH );
    Serial.print("No GPS lock : ");
    Serial.println( gps.satellites.value() );
    return;
  } 
  
  digitalWrite(led , LOW);

  char lat[10];
  dtostrf( gps.location.lat(), 2, 6, lat);
  char lng[10];
  dtostrf( gps.location.lng(), 2, 6, lng);
  char spd[5];
  dtostrf( gps.speed.mph(), 2, 2, spd);
  char course[5];
  dtostrf( gps.course.deg(), 2, 2, course);
  
   // Create the start of the csv string
  sprintf(data, "%d/%d/%d %02d:%02d:%02d, %s,%s, %s mph,%s degrees, %d satellites", 
    gps.date.day(), gps.date.month() , gps.date.year(),
    gps.time.hour(), gps.time.minute(), gps.time.second(),
    lat, lng ,spd, course ,gps.satellites.value()  );

  Serial.println(data);
}
