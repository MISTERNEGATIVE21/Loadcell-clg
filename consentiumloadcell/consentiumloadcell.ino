// Include WiFiManager Library
#include <WiFiManager.h>
#include <HX711.h>
#include <Wire.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); //set the LCD address to 0x27 for a 16 chars and 2 line display
#include "HX711.h"

HX711 scale;

uint8_t dataPin = 12;
uint8_t clockPin = 13;

uint32_t start, stop;
volatile float weight;

void setup() {
 
  // Setup Serial Monitor
  Serial.begin(115200);
 
  // Create WiFiManager object
  WiFiManager wfm;
 
  // Supress Debug information
  wfm.setDebugOutput(false);
 
  // Remove any previous network settings
  wfm.resetSettings();
 
  // Define a text box, 50 characters maximum
  WiFiManagerParameter custom_text_box("my_text", "Enter your Api Key", "default string", 50);
 
  // Add custom parameter
  wfm.addParameter(&custom_text_box);
 
  if (!wfm.autoConnect("ESP32TEST_AP", "password")) {
    // Did not connect, print error message
    Serial.println("failed to connect and hit timeout");
 
    // Reset and try again
    ESP.restart();
     Serial.begin(115200);
  
  // Set up the HX711 and LCD
 scale.tare();
  lcd.begin();
  lcd.backlight();

  // Print a welcome message to the LCD
  lcd.setCursor(0, 0);
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("LoadCell Test");

  delay(6000);
 
    scale.begin(dataPin, clockPin);
  // calib = reading / kown weight 
  // TODO find a nice solution for this calibration..
  // load cell factor 20 KG
  // scale.set_scale(127.15);
  // load cell factor 5 KG
  float calib= -(74695/200);// factor here
  scale.set_scale(calib);       // TODO you need to calibrate this yourself.
  // reset the scale to zero = 0
  scale.tare();
   
    delay(1000);
  }
 
  // Connected!
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
 
  // Print custom text box value to serial monitor
  Serial.print("Custom text box entry: ");
  Serial.println(custom_text_box.getValue());
}
 
void loop() {
 
   weight = scale.get_units(5);
  // Print the weight reading to the serial monitor
  Serial.print("Weight: ");
  Serial.println(weight, 2);

  // Print the weight reading to the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weight: ");
  lcd.print(weight, 2);
  lcd.print(" g");

  delay(500);
 
}