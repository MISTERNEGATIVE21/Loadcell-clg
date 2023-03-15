// Calibrating the load cell
#include <Arduino.h>
#include "HX711.h"
#include <Wire.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); //set the LCD address to 0x27 for a 16 chars and 2 line display
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
scale.tare();
  lcd.begin();
  lcd.backlight();

  // Print a welcome message to the LCD
  lcd.setCursor(0, 0);
  lcd.print("Calibration");
  lcd.setCursor(0, 1);
  lcd.print("LoadCell Test");
  
  delay(5000);
}

void loop() {

  if (scale.is_ready()) {
    scale.set_scale(); 
    lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("remove weights  ");   
    Serial.println("Tare... remove any weights from the scale.");
    delay(10000);
    scale.tare();
    lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Tare Done: ");
    Serial.println("Tare done...");
    lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Place known ");
  lcd.setCursor(0,1);
  lcd.print("Weights : ");
    Serial.print("Place a known weight on the scale...");
    delay(10000);
    long reading = scale.get_units(10);
    lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print("Results");
   lcd.setCursor(0, 1);
   lcd.print(reading);
   
    Serial.print("Result: ");
    Serial.println(reading);
    delay(9000);
  } 
  else {
    lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print("HX711 not found");
    Serial.println("HX711 not found.");
  }
  delay(1000);
}

//calibration factor will be the (reading)/(known weight)
