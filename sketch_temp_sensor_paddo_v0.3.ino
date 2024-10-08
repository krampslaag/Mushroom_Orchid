#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Wire.h>
#include <AutoPID.h>
#include <GyverDimmer.h> 

#define DHTPIN 4  
#define DHTTYPE DHT22   // DHT 22  (AM2302)

#define TEMP_READ_DELAY 3000

#define D_PIN 5

#define PushX_PIN A1
#define PushY_PIN A0
#define PUSH_BUTTON_DELAY 250

#define Waterspray_PIN 8
#define RELAY_DELAY 3500
#define WATER_SPRAY_DELAY 10000

#define OUTPUT_MIN 0
#define OUTPUT_MAX 255
#define KP 40
#define KI 1
#define KD 0

bool PushSWselect = 0;
int  PushValue;
int PushSWvalue;
bool PushTimer;

unsigned char sync = 6; //sync pin
unsigned char channel_1 = 9;

int DimmeroutputVal;

float vochtSet = 95;
float vocht;
int Vochtpoint;

float TemperatureCheck;
float TemperatureCheckF;
int TemperatureCheckten;
float setPointten;

unsigned long lastTempUpdate;
unsigned long LastVochtUpdate;

unsigned long current = 0;
unsigned long last = 0;
int mode;
bool ChangeValue = 0;

unsigned long lastPushUpdate = 0;

bool watersprayActive = 0;
bool relayPulse = 0;
unsigned long lastWatersprayDelay = 0;
unsigned long lastRelayPulse = 0;

bool WaterNeeded = 0;
bool WatersprayTimer = 0;
bool WatersprayState = 0;
bool WatersprayTimerDelay = 0;

double temperature, setPoint = 240, outputVal;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows
AutoPID myPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

DimmerBres<D_PIN> dim; 

void setup()
{
  pinMode(PushX_PIN, INPUT);
  pinMode(Waterspray_PIN, OUTPUT);
  pinMode(PushY_PIN, INPUT);
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  lcd.init(); // initialize the lcd
  lcd.backlight();
  dht.begin();
  while (!updateTemperature()) {} ;
  myPID.setBangBang(4);
  //set PID update interval to 4000ms
  myPID.setTimeStep(4000);
  last = millis();
  attachInterrupt(0, isr, RISING);  // D2 == 0
}

void isr() {
  dim.tick(); // вызывать тик в прерывании детектора нуля
}

bool updateTemperature() {
  if ((millis() - lastTempUpdate) > TEMP_READ_DELAY) {
    TemperatureCheck = dht.readTemperature(); //get temp reading
    vocht = dht.readHumidity();
    lastTempUpdate = millis();
    TemperatureCheckF = dht.convertCtoF(TemperatureCheck);
    TemperatureCheckten = TemperatureCheck*10;
    temperature = TemperatureCheckten;
    if (isnan(TemperatureCheck)) {
    Serial.println("Failed to read from DHT sensor!");
    temperature = 240;
    return;
  }
    return true;
  }
  return false;
}

void updateSetPoint(){
  if ((millis() - lastPushUpdate) > PUSH_BUTTON_DELAY) {
    lastPushUpdate = millis();
    PushTimer = 1;
    }
  if (PushTimer == 1){
    PushSWvalue = analogRead(PushX_PIN);
    PushValue = analogRead(PushY_PIN);
  }
  if (PushSWvalue > 900 || PushSWvalue < 100 ){
    PushSWselect = !PushSWselect;
    PushTimer = 0;
    }
  if (PushSWselect == 1 && PushTimer == 1){
    if (PushValue > 1000) {
      setPoint = setPoint + 1;
      PushTimer = 0;
      };
    if (PushValue < 100) {
      setPoint = setPoint - 1;
      PushTimer = 0;
      }; 
 }
  if (PushSWselect == 0 && PushTimer == 1) {
    if (PushValue > 1000) {
      vochtSet = vochtSet + 1;
      PushTimer = 0;
      };
    if (PushValue < 100) {
      vochtSet = vochtSet - 1;
      PushTimer = 0;
      };
  }  
  setPointten = setPoint/10;
}

void UpdateVochtpoint(){
  if((millis() -  LastVochtUpdate) > 1000) {
    LastVochtUpdate = millis();
    Vochtpoint = vocht;
  }
}

void Watersprayactive(){
  if(WatersprayTimer == 0 && WatersprayTimerDelay == 0){
    digitalWrite(Waterspray_PIN, LOW);
    if ((millis() - lastWatersprayDelay) > WATER_SPRAY_DELAY) {
        lastWatersprayDelay = millis();
        WatersprayTimer = 1;
    }
  }
  if ((vocht < vochtSet) && WatersprayTimer == 1  ){
      WaterNeeded = true;
      WatersprayTimerDelay = 0;
    }
  if (WaterNeeded == true && relayPulse == false){
      watersprayActive = true;
      lastRelayPulse = millis();
      relayPulse = 1;
    }
  if (watersprayActive == true) {
    digitalWrite(Waterspray_PIN, HIGH);
    if ((millis() - lastRelayPulse) > RELAY_DELAY) {
      WatersprayTimer = 0;
      relayPulse = 0;
      watersprayActive = 0;
      WaterNeeded = 0;
      }
    }
}

void loop(){

  updateTemperature();
  updateSetPoint();
  Watersprayactive();
  myPID.run();
  DimmeroutputVal = outputVal;
  dim.write(DimmeroutputVal); //call every loop, updates automatically at certain time interval
  lcd.clear();                 // clear display
  lcd.setCursor(0, 0);         // move cursor to   (0, 0)
  lcd.print("temp: ");
  lcd.print(TemperatureCheck, 1);
  lcd.print("");
  lcd.print("->");
  lcd.print(setPointten, 1);
  // print message at (0, 0)
  lcd.setCursor(0 ,1);         // move cursor to   (2, 1)
  lcd.print("v: "); 
  lcd.print(vocht, 0); 
  lcd.print("% ->");
  lcd.print(vochtSet, 0);
  // print message at (2, 1)                 // display the above for two seconds
      // display the above for two seconds
  Serial.println(F("temp"));
  Serial.println(TemperatureCheck);
  Serial.println("DimmerOutputVal");
  Serial.println(DimmeroutputVal);
   Serial.println("outputVal");
  Serial.println(outputVal); 
  delay(100);
}
