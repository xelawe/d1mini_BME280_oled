#define serdebug
#ifdef serdebug
#define DebugPrint(...) {  Serial.print(__VA_ARGS__); }
#define DebugPrintln(...) {  Serial.println(__VA_ARGS__); }
#else
#define DebugPrint(...) { }
#define DebugPrintln(...) { }
#endif

#include "cy_wifi.h"
#include "cy_ota.h""
#include "cy_weather.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Ticker.h>

#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library


#define PIN_RESET 255  //
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D

Adafruit_BME280 bme; // I2C

MicroOLED oled(PIN_RESET, DC_JUMPER); // Example I2C declaration

const char* gv_hostname = "D1BMEOLED";
float gv_humidity;
float gv_temp;
float gv_press;

Ticker TickMeas;
boolean gv_TickMeas;

void HandleTickMeas( ){
  gv_TickMeas = true;
}

void setup() {
#ifdef serdebug
  Serial.begin(115200);
#endif
  DebugPrintln("\n" + String(__DATE__) + ", " + String(__TIME__) + " " + String(__FILE__));

  // These three lines of code are all you need to initialize the
  // OLED and print the splash screen.

  oled.begin();     // Initialize the OLED
  oled.flipVertical(true);
  oled.flipHorizontal(true);
  oled.clear(PAGE); // Clear the display's internal memory
  oled.clear(ALL);  // Clear the library's display buffer
  oled.setFontType(0); // set font type 0, please see declaration in SFE_MicroOLED.cpp
  oled.setCursor(0, 0); // points cursor to x=0 y=0
  oled.println("WiFi conn");
  oled.display();   // Display what's in the buffer (splashscreen)

  wifi_init(gv_hostname);

  oled.clear(PAGE); // Clear the display's internal memory
  oled.clear(ALL);  // Clear the library's display buffer
  oled.setFontType(0); // set font type 0, please see declaration in SFE_MicroOLED.cpp
  oled.setCursor(0, 0); // points cursor to x=0 y=0
  oled.println("WiFi OK");
  oled.display();   // Display what's in the buffer (splashscreen)

  delay(500);

  init_ota(gv_hostname);

  bool status = bme.begin(0x76);
  if (!status) {
    DebugPrintln("Could not find a valid BME280 sensor, check wiring!");
    //while (1);
  }
  delay(100); // let sensor boot up

  do_sensor();
  gv_TickMeas = false;
  TickMeas.attach(10, HandleTickMeas);
}

void loop() {
  check_ota();

  if (gv_TickMeas == true) {

    do_sensor();

    display_data();
    gv_TickMeas = false;

  }

  delay(500);

}

void display_data(){
  oled.clear(PAGE);  // Clear the buffer
  oled.setFontType(0); // set font type 0, please see declaration in SFE_MicroOLED.cpp
  oled.setCursor(0, 0); // points cursor to x=0 y=0


  oled.print("T: ");
  oled.print(gv_temp,1);
  oled.println(" \tC");

  oled.print("H: ");
  oled.print(gv_humidity,0);
  oled.println(" % ");

  oled.print("P: ");
  oled.print(gv_press,0);
  oled.println(" hPa");

  oled.display(); // Draw the memory buffer
}

void do_sensor() {

  get_bme280();

}

void get_bme280() {
  gv_temp = bme.readTemperature();
  DebugPrint("Temperature: ");
  DebugPrint(gv_temp);
  DebugPrint(" *C ");

  //Serial.print("Pressure = ");
  //Serial.print(bme.readPressure() / 100.0F);
  //Serial.println(" hPa");
  gv_press = bme.readPressure() / 100;

  //Serial.print("Approx. Altitude = ");
  //Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  //Serial.println(" m");


  gv_humidity = bme.readHumidity();
  DebugPrint("Humidity: ");
  DebugPrint(gv_humidity);
  DebugPrint(" % ");
  DebugPrintln();
}

