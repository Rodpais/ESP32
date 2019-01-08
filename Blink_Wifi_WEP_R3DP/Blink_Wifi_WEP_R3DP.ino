/*
 * WiFi Scan + Web Server +  LED chassi Blink
 * 
 * Rodrigo Dias Pais
 * Project Details http://www.r3dp.com.br/tutoriais/iot
 * 
 *  A simple web server that lets you blink  the  LED chassi of ESP32 (LED_BUILTIN) via the web.
 *  This sketch will print the IP address of your WiFi Shield (once connected)
 *  to the Serial monitor. From there, you can open that address in a web browser
 *  to turn on and off the LED chassi (LED_BUILTIN).
 * If the IP address of your shield is yourAddress:
 *  http://yourAddress/H (high)turns the LED on
 *  http://yourAddress/L  (low) turns the LED off
 * 
 * 
 * 
 * based on the sources of:
 * Scan Wifi:
 * Nuno Santos: https://techtutorialsx.com/2017/06/29/esp32-arduino-getting-started-with-wifi/ 
 * 
 * Web Server:
 * created for arduino 25 Nov 2012
 * by Tom Igoe
 *
 * ported for sparkfun esp32 
 * 31.01.2017 by Jan Hendrik Berlin
 * 
 * and Blink example:
 *  modified 8 May 2014
 *  by Scott Fitzgerald
 *
 *  modified 2 Sep 2016
 *  by Arturo Guadalupi
 *
 *  modified 8 Sep 2016
 *  by Colby Newman
*/



#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "R3DP";
const char* password =  "Zeca814222";

WiFiServer server(80);

// Client variables 
char linebuf[80];
int charcount=0;
 
String translateEncryptionType(wifi_auth_mode_t encryptionType) {
 
  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}
 
void scanNetworks() {
 
  int numberOfNetworks = WiFi.scanNetworks();
 
  Serial.print("Number of networks found: ");
  Serial.println(numberOfNetworks);
 
  for (int i = 0; i < numberOfNetworks; i++) {
 
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));
 
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));
 
    Serial.print("MAC address: ");
    Serial.println(WiFi.BSSIDstr(i));
 
    Serial.print("Encryption type: ");
    String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
    Serial.println(encryptionTypeDescription);
    Serial.println("-----------------------");
 
  }
}
 
void connectToNetwork() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
}
 
void setup() {
 //Initialize serial and wait for port to open:
  Serial.begin(115200);
   while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);

  delay(10);
 
  scanNetworks();
  connectToNetwork();
 
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  Serial.println(ssid);
 
  //WiFi.disconnect(true);
  //Serial.println(WiFi.localIP());
  
  server.begin();

}
 
void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<!DOCTYPE HTML><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            // the content of the HTTP response follows the header:
            client.println("<div style=\"color: #009191;\">");

            client.print("Click <a href=\"/H\">here</a> to turn the LED on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED ooff.<br>");
            client.println("</p></div>");
          client.println("</body></html>");
          
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
    }
  }
