#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "BMP085Reader.h"

//
//    Example Code for ESP8266 and BMP085 using Arduino IDE
//    Updated by G.Pimblott
//    Created: 27 Jan 2016 (G.Pimblott)
//
//    1) Connects to a specified wireless network using DHCP (IP output to serial)
//    2) Starts a webserver on port 80
//    3) Responds to requests to the root URL with sensor readings from a connected BMP085
//
//    By default it expects the BMP085 to use sda(4) and scl(5) on an NodeMCU development board.
//   

const int led = 2;

// Update these with your network settings
const char* ssid = "Change-me";
const char* password = "Change-me";

// Create the webserver
ESP8266WebServer server(80);

// Setup the BMP085 reader
BMP085Reader bmpReader;


// Process a request for the data
void handleTemp() {
  digitalWrite(led, 1);
  String message = "Hello - Here are the current readings\n\n";

  bmpReader.readValues();
  
  char temp[10];
  dtostrf( bmpReader.getTemperature(), 2, 2, temp);
 
  message += "Temperature: ";
  message += temp;
  message += " dec C\n";

  message += "Pressure: ";
  message += bmpReader.getPressure();
  message += " Pa\n";

  message += "Altitude: ";
  message += bmpReader.getAltitude();
  message += " m\n";
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" deg C");
  Serial.print("Pressure: ");
  Serial.print(bmpReader.getPressure(), DEC);
  Serial.println(" Pa");
  Serial.print("Altitude: ");
  Serial.print(bmpReader.getAltitude(), 2);
  Serial.println(" m");
  Serial.println();

  server.send(200, "text/plain", message);
  digitalWrite(led, 0);
}

// Page not found message
void handleNotFound()
{
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
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

//
// Initial setup routine
//
// 1) Open serial port for logging
// 2) Initialise the webserver
// 3) Initialise the BMP085
//
void setup()
{
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Starting...");

  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  
  // Initialise the web server
  initWebServer(ssid , password);

  server.on("/", handleTemp);
  server.onNotFound(handleNotFound);
  server.begin();
  
  Serial.println("HTTP server started");

  // Initialise the BMP085
  bmpReader.init(4,5);
  
}


// Keep looping handling new HTTP connections
void loop()
{
  server.handleClient();
}

