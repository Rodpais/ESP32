/*
 * ESP32 + SPI OLED digital - Barometer using bmp180 and Humidity using DHT11
 * Autor: Rodrigo Dias  
 * Complete Project Details http://www.r3dp.com.br/iot
 * http://www.r3dp.com.br
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>
#include <gfxfont.h>
#include <Adafruit_Sensor.h>    /* Adafruit's Sensor incl. BMP085/180 DHT11*/
#include <stdio.h>
#include <time.h>
 

//#include <WiFi.h>
//#include <WiFiClient.h>
//#include <WiFiServer.h>
//#include <WiFiUdp.h>

/*
const char* ssid     = "R3DP";
const char* password = "Zeca814222";
String CityID = "253394"; //Sparta, Greece
String APIKEY = "yourAPIkey";
#define ALTITUDE 216.0 // Altitude in Sparta, Greece
*/

/***************************************************
 DHT11 
****************************************************/
// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 23  
#define DHTTYPE DHT11 
//DHT dht(DHTPIN, DHTTYPE);
DHT_Unified dht(DHTPIN, DHTTYPE);
int localHum = 0;
int localTemp = 0;


uint32_t delayMS;

// based on adafruit example
/***************************************************
  This is an example for the BMP085 Barometric pres & Temp Sensor
  Designed specifically to work with the Adafruit BMP085 Breakout
  ----> https://www.adafruit.com/products/391
  This is an example for our Monochrome OLEDs based on SSD1306 drivers
  for a 128x64 size display using SPI to communicate
  4 or 5 pins are required to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
 ****************************************************/


#include <Adafruit_BMP085_U.h>  /* Adafruit's Sensor incl. BMP085/180 */

/* The BMP085/BMP180 uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.
   
   To use this driver you need to download the Adafruit_Sensor
   library and include it in your libraries folder. Reference
   Adafruit's tutorial on how to install libraries if you are not familiar.
   
   If you have multiple I2C sensors you should also assign a unique ID(I2C)
   to this sensor for use with the Adafruit Sensor API so that you can identify 
   this particular sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (10085
   is used in this example).
   
   Connections for the Bosch BMP085/ BMP180 Sensor
   ===========
   Connect Sensor_SDA to analog 4 for arduino or Gpio21 for ESP32
   Connect Sensor_SCL to analog 5 for arduino or Gpio22 for ESP32
   Connect Sensor_VCC to 3.3V DC (5V tolerant)
   Connect Sensor_GROUND to common ground
*/

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

/* 
   To use this OLED driver you will need to download the Adafruit
   OLED library and include it in your libraries folder. I used Adafruit's
   128x64 OLED. It uses the SSD1306 driver and this sketch uses an SPI
   interface to the OLED for a speedy refresh!
   
   Connections for the Adafruit 128x64 SPI OLED
   ===========
   Connect OLED_MOSI(DATA) 9
   Connect OLED_CLS(Clock) 10
   Connect OLED_DC(SAO) 11
   Connect OLED_RESET 13
   Connect OLED_CS 12
   Connect OLED_VDD(Vin) to 3.3V DC (5V tolerant)
   Connect OLED_GROUND to common ground
*/

// If using software SPI (the default case):
#define OLED_MOSI   2
#define OLED_CLK   15
#define OLED_DC    5
#define OLED_CS    18
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
//Adafruit_SSD1306 display(OLED_RESET);

/* Uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
*/

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

/**************************************************************************/
/*
    Displays some information about this sensor BMP180/BMP085 and this DHT11 from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/

void displaySensorDetails(void)
{
  //first BMP180/BMP085
  sensor_t sensor;
  bmp.getSensor(&sensor);
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(2,0);
  display.println("Pressure");
  display.println("--------------------");
  display.print  ("Sensor:   "); display.println(sensor.name);
  display.print  ("Drv. Ver: "); display.println(sensor.version);
  display.print  ("Unique ID:"); display.println(sensor.sensor_id);
  display.print  ("Max Value:"); display.print(sensor.max_value); display.println(" hPa");
  display.print  ("Min Value:"); display.print(sensor.min_value); display.println(" hPa");
  display.print  ("Res.:     "); display.print(sensor.resolution); display.println(" hPa");  
  //display.println("--------------------");
  //display.println("");
  display.display();
  delay(5000);
  display.clearDisplay();   // cleanup the display before leaving 
  display.display();

  //second DHT11/DHT12
  //sensor_t sensor;
  //dht.getSensor(&sensor);
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(2,0);
  display.println("Humidity");
  display.println("--------------------");
  display.print  ("Sensor:    "); display.println(sensor.name);
  display.print  ("Drv. Ver:  "); display.println(sensor.version);
  display.print  ("Unique ID: "); display.println(sensor.sensor_id);
  display.print  ("Max Value: "); display.print(sensor.max_value); display.println(" %");
  display.print  ("Min Value: "); display.print(sensor.min_value); display.println(" %");
  display.print  ("Res.:      "); display.print(sensor.resolution); display.println(" %");  
  //display.println("--------------------");
  //display.println("");
  display.display();
  delay(5000);
  display.clearDisplay();   // cleanup the display before leaving 
  display.display();

}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)  
{
  Serial.begin(9600);
 
  /* by default, we'll generate the high voltage from the 3.3v line internally! (neat!) */
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(4000);

  // Clear the buffer.
  display.clearDisplay();
  
  /* Set up the OLED display initialization characteristics. */
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  /* Display basic information on this temp/barometer sensor 
  * (void displaySensorDetails(void))
  */
  displaySensorDetails();

  /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    display.setCursor(0,0);
    display.println("No BMP detected....Check your wiring or I2C ADDR!");
    display.display();
    while(1);
  }  
  /* Inform user setting up sensor.           
  display.setCursor(0,0);
  display.println("");
  display.println("Setting Up BOSCH");
  display.println("");
  display.println("BMP085/BMP180 Sensor");
  display.display();
  delay(3000);
  */ 
} 

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/

void loop(void)  
{ int x=0;
  int y=0;
    
  //localHum = dht.humidity();
  //float humIni = localHum;
 
  delay(delayMS);
  // Get humidity event and display its value.
  sensors_event_t event;  
 // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))  // Check if any reads failed and exit early (to try again).
  {
    display.println("Falha ao ler Sensor DHT!");
    //localHum = humIni;
    return;
  }
  
  /* Get a new sensor event */ 
  //sensors_event_t event;
  bmp.getEvent(&event);
  display.clearDisplay(); 
  display.setTextSize(1);
  display.setTextColor(WHITE);

  /* Display the results (barometric pressure is measure in hPa) */
  if (event.pressure)
  {
    // Now send the same data to the display

    display.setCursor(0,0);
    display.print("BAROM.: ");
    display.print(event.pressure);
    display.println(" hPa");
    //display.println("");
    //display.display();
 
     /* Display atmospheric pressue in inHg */
    float PRESSURE_HG;
    /* Convert HPa to inches Mercury       */
    PRESSURE_HG= (event.pressure * 0.02952998751);
    //display.print("BAROM.: ");
    display.setCursor (47 + x, 11 + y);
    display.print(PRESSURE_HG);
    display.print(" inHg ");
    /* Decimal 94 is an up arrown          */
    display.write(24);
    /* Decimal 118 is a down arrow         */
    // Serial.write(118);
    //display.println();
    display.println("");
    //display.display();
    
    // Display temperature in Celsius
    // bmp.getTemperature(&temperature);
    int porcentagem = 37;
    int grau = 167;
    /* First we get the current temperature from the BMP085 */    
    // Print Temperature in Celsius 
    float temperature;
    bmp.getTemperature(&temperature);
    display.setCursor (0 + x, 23 + y);    
    display.print("TEMP: ");            // Printing temp in Celsius
    display.print(temperature);
    display.print("C / ");
    //display.write(67);
    //display.write(grau);

    //display.println("");
    //display.display();
    // Serial.print("Temperature: ");
    // Serial.print(temperature);
    // Serial.println(" C");
 
   /* Now lets convert to Farenheit and Dispaly */
    
   /* Display temperature in Farenheit */
    float farenheit;
    farenheit = (temperature * 1.8) +32;
    //display.print("TEMP:");
     display.setCursor (86 + x, 23 + y);
    display.print(farenheit);
    display.println("F");
    display.println(""); 
    // Print Temperature in Farenheit
    // Serial.print(Farenheit);
    // Serial.println(" F"); 
     
    /* Then convert the atmospheric pressure, SLP and temp to altitude    */
    /* Update this next line with the current SLP for better results      */
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    display.setCursor (0 + x, 36 + y);  
    display.print("ALTD: "); 
    display.print(bmp.pressureToAltitude(seaLevelPressure,event.pressure, temperature)); 
    display.println(" m");

    /* Display Humidity in % from DHT11 */
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
    display.setCursor (0 + x, 48 + y);
    display.print("Hum.:");
    display.println("Error humidity!");
    }
    else {
    display.setCursor (0 + x, 48 + y);
    display.print("Hum.:  ");
    display.print(event.relative_humidity);
    display.println(" %");
    }
  
    
     display.display();    
  }
  else
  {
    display.println("Sensors error");
  }
  delay(3000);
}

