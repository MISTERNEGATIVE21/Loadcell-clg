#define BLYNK_TEMPLATE_ID "TMPL3V43SnUX7"
#define BLYNK_TEMPLATE_NAME "iotscale"
#define BLYNK_AUTH_TOKEN   "_i9_U173jz3pm6Rt5jWbya6sbSfS1bQo"
#define BLYNK_PRINT Serial
#include "HX711.h"
#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
String sid; //from user
String pss;// all declared in setup()
float finalcalib; // gloabl for global calibration
int knownweight = 200 ; // supplied weight to be calibrated
HX711 scale;
uint8_t dataPin = 18;
uint8_t clockPin = 19;
int rbutton = 12; // this button will be used to reset the scale to 0.
float weight;
float calibration_factor = (2800.75/200); // for me this vlaue works just perfect 419640

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiManager.h>
void setup() 
{
  Serial.begin(115200);
    scale.begin(dataPin, clockPin);
  pinMode(rbutton, INPUT_PULLUP); 
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
 
  Wire.begin( ); // use default 22 ,21 for sclk amd sda in esp32
  lcd.begin();
  lcd.setCursor(6,0);
  lcd.print("IOT");
  lcd.setCursor(1,1);
  lcd.print("Weighing Scale");
  delay(5000);
  lcd.clear();
     WiFiManager wfm;
 
  wfm.setDebugOutput(false);

  wfm.resetSettings();

    WiFiManagerParameter sendapi_text_box("my_text", "Enter the BLYNK_TEMPLATE_ID ", "default", 50);
    WiFiManagerParameter boardapi_text_box("my_text", "Enter the BLYNK_TEMPLATE_NAME ", "default", 50);
  // Add custom parameter
  wfm.addParameter(&sendapi_text_box);
  wfm.addParameter(&boardapi_text_box);

  if (!wfm.autoConnect("IOT-WEIGHING_SCALE", "iotscale")) {
    // Did not connect, print error message
    Serial.println("failed to connect and hit timeout");

    // Reset and try again
    ESP.restart();
    delay(1000);
  }
   
  lcd.print("Connecting Wifi");
  
    sid = WiFi.SSID();
    pss = WiFi.psk();
     const char *ssid=(char*)sid.c_str();
     const char *pass=(char*)pss.c_str();
     const char *key = (char*)sendapi_text_box.getValue();
     const char *board_key = (char*)boardapi_text_box.getValue();
 
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  {
  delay(1000);
  Serial.print(".");
  lcd.clear();
  }
  Serial.println("");
  Serial.println("WiFi connected");
  lcd.clear();
  lcd.print("WiFi connected");
  
  delay(2000);
    lcd.clear();
}
 
void loop() 
 
{

  Blynk.run();
 
  measureweight();
  reading();
  Serial.print(digitalRead(rbutton));
  if ( digitalRead(rbutton) == HIGH)
{
  scale.set_scale();
   calibration();
   // tare(); //Reset the scale to 0
}
  
}

 void tare()
{
scale.tare();
    lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Tare Done: ");
    Serial.println("Tare done...");
    lcd.clear();
    delay(5000);
}
void calibration () // calibration test
{
Serial.print(" Enter the Known weight ");
int knownweight = 200;
long reading; 
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
    reading = scale.get_units(10);
    lcd.clear();
  lcd.setCursor(0, 0);


  lcd.print("Results");
   lcd.setCursor(0, 1);
   lcd.print(reading);
   
    Serial.print("Result: ");
    Serial.println(reading);
    delay(9000);
        finalcalib = reading/knownweight;
        calibration_factor=finalcalib;   // pointer to calibration_factor
  } 
  else {
    lcd.clear();
  lcd.setCursor(0, 0);


  lcd.print("HX711 not found");
    Serial.println("HX711 not found.");
  }
  delay(1000);
}




void measureweight() {
  scale.set_scale(calibration_factor);  //Adjust to this calibration factor
  Serial.print("Reading: ");
  weight = scale.get_units(5);
  if (weight < 0) {
    weight = 0.00;
  }
  //Serial.print(scale.get_units(), 2);
  // Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print("Kilogram:");
  Serial.print(weight);
  Serial.print(" Kg");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
  Blynk.virtualWrite(V0, weight);
  Blynk.virtualWrite(V1, weight);
  // Delay before repeating measurement
  delay(100);
}
void reading() {

  lcd.setCursor(0, 0);
  lcd.print("Measured Weight");
  lcd.setCursor(0, 1);
  lcd.print(weight);
  lcd.print(" KG  ");
  delay(5000);
  lcd.clear();
}

