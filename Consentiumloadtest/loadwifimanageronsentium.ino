#include <ConsentiumThingsDalton.h>
#include <WiFiManager.h>
#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
String sid; //from user
String pss;// all declared in setup()
float finalcalib; // gloabl for global calibration
int knownweight = 200 ; // supplied weight to be calibrated
HX711 scale;
ConsentiumThings board;   // create ConsentiumThing object
uint8_t dataPin = 18;
uint8_t clockPin = 19;
const char *key;
     const char *board_key ;
int rbutton = 12; // this button will be used to reset the scale to 0.
float weight;
float calibration_factor = (2800.75/200); // for me this vlaue works just perfect 419640
 
void setup() 
{
  Serial.begin(115200);
   Wire.begin(21, 22);
  lcd.begin();
  lcd.setCursor(6,0);
  lcd.print("IOT");
  lcd.setCursor(1,1);
  lcd.print("Weighing Scale");
  delay(2000);
  
     WiFiManager wfm;

  
  wfm.setDebugOutput(false);

  wfm.resetSettings();

    WiFiManagerParameter sendapi_text_box("my_text", "Enter the Send API KEY ", "default", 50);
    WiFiManagerParameter boardapi_text_box("my_text", "Enter the API KEY ", "default", 50);
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
  lcd.clear();
  scale.begin(dataPin, clockPin);
  pinMode(rbutton, INPUT_PULLUP); 
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  board.begin();   // init. IoT boad
 
 
  lcd.print("Connecting Wifi");
  
  
    board.begin();   // init. IoT boad
     //read the connected WiFi SSID and password
    sid = WiFi.SSID();
    pss = WiFi.psk();
     const char *ssid=(char*)sid.c_str();
     const char *pass=(char*)pss.c_str();
     const char *key = (char*)sendapi_text_box.getValue();
     const char *board_key = (char*)boardapi_text_box.getValue();

  board.initWiFi(ssid, pass);  // begin WiFi connection
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
}
 
 
  void measure(){
  
    scale.set_scale(calibration_factor); //Adjust to this calibration factor
    weight = scale.get_units(5); 
  
    // lcd.setCursor(0, 0);
    // lcd.print("Measured Weight");
    // lcd.setCursor(0, 1);
    // lcd.print(weight);
    // lcd.print(" KG  ");
    // delay(2000);
    // lcd.clear();
    
    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.println(" KG");
    Serial.println();
    double sensor_val[] = {weight};  // sensor data array
    String info_buff[] = {"Weight"}; // sensor info. array
    
    int sensor_num = sizeof(sensor_val)/sizeof(sensor_val[0]); // number of sensors connected 
    
  board.sendREST(key, board_key, sensor_val, info_buff, sensor_num, LOW_PRE); // send over REST with delay with desired prescision
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

  void reading() {

  lcd.setCursor(0, 0);
  lcd.print("Measured Weight");
  lcd.setCursor(0, 1);
  lcd.print(weight);
  lcd.print(" KG  ");
  delay(5000);
  lcd.clear();
}
void loop() 
 
{  
  measure();
  reading();
  Serial.print(digitalRead(rbutton));
  if ( digitalRead(rbutton) == HIGH)
{
  scale.set_scale();
   calibration();
   // tare(); //Reset the scale to 0
}
  
}

 