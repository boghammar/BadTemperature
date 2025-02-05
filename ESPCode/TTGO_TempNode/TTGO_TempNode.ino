/** TTGO_TempNode - For a Lilygo TTGO LORA32 V2.1 with
 * - OLED Display 128x64
 * - 
 */
//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Arduino.h"

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for BMP280
#include <Adafruit_BMP280.h>

//libraries for OneWire
#include <OneWire.h>
#include <DallasTemperature.h>

// project libraries
#include "StateMachine.h"
// -------------------------------------------------------- CLASS SENSOR
class Sensor {
public:
  Sensor() { _ix = -1;_send=false;_threasHold=-1;_value=-1;_unit="X";}

  String toString() {
    String me = "Sensor id=";
    me += _ix;
    me += " Val=";
    me += _value;
    me += " Unit=";
    me += _unit;
    me += " TH=";
    me += _threasHold;

    return me;
  }
  int _ix;
  float _threasHold;
  float _value;
  String _unit; 
  bool _send;
};

// -------------------- Define the pins used by the OLED pins
#define OLED_SDA 21 //4
#define OLED_SCL 22 //15 
#define OLED_RST -1 //16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// -------------------- Define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23 //14
#define DIO0 26

// -------------------- Define the pins used by the OneWire pins
#define ONE_WIRE_BUS 12

// -------------------- Define the LoRa settings
#define BAND 868000000 //866E6

// -------------------- Other defines
#define LOOPFREQUENCY 5000        // How often shall the loop execute
#define TIME_TO_SLEEP  10         /* Time ESP32 will go to sleep (in seconds) */
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */

// ---------------------------------------------------------------------------- Global Variable decl
RTC_DATA_ATTR int bootCount = 0;   /* store data in RTC memory to use it over reboots*/

String loraMsgRcvd = "";
int rssi = 0;

long lastLoopMilli = -LOOPFREQUENCY; // Just so that measurements are taken once

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

Adafruit_BMP280 bmp; // I2C

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasSensors(&oneWire);
int numberOfDevices = 0;
int numberOfSensors = 0;

// ---------------------------------------------------------------------------- 
// Sensor data
// ---------------------------------------------------------------------------- 
#define NUM_SENSORS 3
#define NO_DATA -200.0

int nodeID = 2;                 // LORA Node ID
int foundSensors = 0;
Sensor sensors[NUM_SENSORS];
RTC_DATA_ATTR float lastSensorValues[NUM_SENSORS] = {NO_DATA, NO_DATA, NO_DATA}; // Need to keep these between sleeps
//float sensorValueThreashold[NUM_SENSORS] = {0.5, 1.0, 0.5};
//String sensorUnits[NUM_SENSORS] = {"C","hPa","C"};

StateMachine sm;

// ---------------------------------------------------------------------------- 
// Function prototypes
// ---------------------------------------------------------------------------- 
void setupOLED();

void setupLORA();
void onReceive(int packetSize);
void sendMessage(String message);

void setupBMP();
void getBMPMeasures();

void setupDallas();
void getDallasMeasures();

bool newValuesObtained();
void sendNewValues();

bool every(int frequency);
void goToSleep();
void print_wakeup_reason();
// ---------------------------------------------------------------------------- 
void setup() {
  Serial.begin(115200);
  while(!Serial);

  char ssid[23];
  snprintf(ssid, 23, "ESP-%llX", ESP.getEfuseMac());
  Serial.print("---------------------------------------------------------- LoRa with TTGO LoRa32 V2.1 ");
  Serial.println(ssid);
  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  setupOLED();
  setupLORA();
  setupBMP();
  setupDallas();
  for (int ix=0; ix < foundSensors; ix++) {
    Serial.println(sensors[ix].toString());
  }
  Serial.println("---------------------------------------------------------- LoRa with TTGO LoRa32 V2.1 - Setup done");
}

// ---------------------------------------------------------------------------- 
void loop() {
  if (every(LOOPFREQUENCY)) {
    Serial.print("------------------------------ Next loop ");
    Serial.println(sm.state());
    switch (sm.currentState) {
      case State::SAMPLEDATA:
        getBMPMeasures();
        getDallasMeasures();
        Serial.println("Measurement done");
        sm.sampleDataDone();
        break;
      case State::SENDDATA:
        if (newValuesObtained()) {
          // Send the values
          sendNewValues();
          sm.sendDataDone();
        } else {
          sm.sendNoDataDone();
        }
        break;
      case State::RECEIVEMESSAGE:
        // Do nothing, handled in receive function
        break;
      case State::SLEEP:
        goToSleep();
        break;
    }
  }
}

// ---------------------------------------------------------------------------- 
// LORA FUNCTIONS
// ---------------------------------------------------------------------------- 
void setupLORA() {
  // ------------------ SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  // ------------------ setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1) delay(100); 
  }
  LoRa.onReceive(onReceive);
  Serial.println("LORA init done");
}
// ---------------------------------------------------------------------------- 
void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  loraMsgRcvd = "";

  while(LoRa.available()) {
    loraMsgRcvd += (char) LoRa.read();
  }

  rssi = LoRa.rssi();

  Serial.printf("LoRa in: %s RSSI: %d", loraMsgRcvd.c_str(), rssi);
  sm.receiveMessageDone();
}
// ---------------------------------------------------------- Send a message
void sendMessage(String message) {
  //String msgId = getMsgId();
  // Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
}
// ---------------------------------------------------------------------------- 
// OLED SETUP
// ---------------------------------------------------------------------------- 
void setupOLED() {

  // ------------------ reset OLED display via software Not necessary?
  // pinMode(OLED_RST, OUTPUT);
  // digitalWrite(OLED_RST, LOW);
  // delay(20);
  // digitalWrite(OLED_RST, HIGH);

  // ------------------ initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    while (1) delay(100); 
  }
  Serial.println("OLED init done");

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA TMPN B#");
  display.print(bootCount);
  display.display();
}
// ---------------------------------------------------------------------------- 
// BMP SETUP
// ---------------------------------------------------------------------------- 
void setupBMP() {

  if (!bmp.begin(BMP280_ADDRESS_ALT)) {
    Serial.println("BMP280 allocation failed");
    Serial.print("SensorID=0x");Serial.println(bmp.sensorID(), 16);
    while (1) delay(100); 
  }

  /* Setup the device in forced mode for lowest power consumption. Comment shows defaults*/
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode MODE_NORMAL. */
                  Adafruit_BMP280::SAMPLING_X1,     /* Temp. oversampling SAMPLING_X2*/
                  Adafruit_BMP280::SAMPLING_X1,     /* Pressure oversampling SAMPLING_X16*/
                  Adafruit_BMP280::FILTER_OFF,      /* Filtering.FILTER_X16 */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. STANDBY_MS_500 */

  sensors[foundSensors]._ix = foundSensors;
  sensors[foundSensors]._threasHold = 0.5;
  sensors[foundSensors]._unit = "C";
  foundSensors++;
  sensors[foundSensors]._ix = foundSensors;
  sensors[foundSensors]._threasHold = 1;
  sensors[foundSensors]._unit = "hPa";
  foundSensors++;
  Serial.println("BMP init done");
}
// ---------------------------------------------------------------------------- 
void getBMPMeasures() {
  int ip = 0;
  //Serial.printf("BMP measure %d\n", ip++);
  if (bmp.takeForcedMeasurement()) {
    //Serial.printf("BMP measure %d\n", ip++);
    sensors[0]._value = bmp.readTemperature();     
    Serial.printf("BMP    Temp: %f %s ", sensors[0]._value, sensors[0]._unit.c_str());
    sensors[1]._value = bmp.readPressure()/100;
    Serial.printf("Pres: %f %s\n", sensors[1]._value, sensors[1]._unit.c_str());
  } else {
    Serial.printf("BMP    Failed to take forced measurement\n");
  }
}
// ---------------------------------------------------------------------------- 
// Dallas DS18B20 SETUP
// ---------------------------------------------------------------------------- 
void setupDallas() {
  int ix = 0;
  //Serial.printf("DALLAS setup %d\n", ix++);
  dallasSensors.begin(); // Init the DS18B20 sensor
  //Serial.printf("DALLAS setup %d\n", ix++);
  delay(2000);
  //Serial.printf("DALLAS setup %d\n", ix++);
  numberOfDevices = dallasSensors.getDeviceCount();
  //Serial.printf("DALLAS setup %d\n", ix++);
  numberOfSensors = dallasSensors.getDS18Count();
  //Serial.printf("DALLAS setup %d\n", ix++);
  /* get sensor resolution */
  int resolution = -1; //(numberOfSensors > 0 ? dallasSensors.getResolution(/* sensorAddress */ 0) : -1);
  //Serial.printf("DALLAS setup %d\n", ix++);
  Serial.printf("Devices %d Sensors %d Res: %d\n", numberOfDevices, numberOfSensors, resolution);
  if (numberOfSensors > 0) {
    sensors[foundSensors]._ix = foundSensors;
    sensors[foundSensors]._threasHold = 0.5;
    sensors[foundSensors]._unit = "C";
    foundSensors++;
  }
  Serial.println("DALLAS init done");
}
// ---------------------------------------------------------------------------- 
void getDallasMeasures() {
  if (numberOfSensors > 0) {
    dallasSensors.requestTemperatures(); 
    sensors[2]._value = dallasSensors.getTempCByIndex(0);
    Serial.printf("Dallas Temp: %f º%s \n", sensors[2]._value, sensors[2]._unit.c_str());
  } else {
    Serial.printf("Dallas No sensors detected\n");
  }
}
// ---------------------------------------------------------------------------- 
// Sensor data handling
// ---------------------------------------------------------------------------- 
bool newValuesObtained() {
  bool ret = false;
  for (int ix=0; ix< foundSensors; ix++) {
    sensors[ix]._send = false;
    if (lastSensorValues[ix] != sensors[ix]._value) { // Theres a new value obtained, calculate diff
      float diff = sensors[ix]._value - lastSensorValues[ix];
      if (diff < 0) {
        diff = -diff;
      }
      if (diff > sensors[ix]._threasHold) { // So its a change bigger then threashold
        sensors[ix]._send = true;
        ret = true;
      }
    }
  }
  return ret;
}
// ---------------------------------------------------------------------------- 
void sendNewValues() {

  for (int ix=0; ix< foundSensors; ix++) {
    if (sensors[ix]._send) { // They are the same send this one
      Serial.printf("Sending sensor %i %f %s LastValue %f\n", ix, sensors[ix]._value, sensors[ix]._unit.c_str(), lastSensorValues[ix]);
      String msg = "B " + String(bootCount) + " " + String(sensors[ix]._value);
      Serial.println(msg);
      sendMessage(msg);
      LoRa.receive();
      lastSensorValues[ix] = sensors[ix]._value;
    } else {
      Serial.printf("Not Sending sensor %i %f %s LastValue %f\n", ix, sensors[ix]._value, sensors[ix]._unit.c_str(), lastSensorValues[ix]);
    }
  }
}
// ---------------------------------------------------------------------------- 
// UTILITIES
// ---------------------------------------------------------------------------- 
bool every(int frequency) {
  bool ret = (millis() - lastLoopMilli > frequency);
  if (ret) lastLoopMilli = millis();
  return ret;
}
// ---------------------------------------------------------------------------- 
void goToSleep() {
    Serial.println("Preparing to sleep now");
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    LoRa.sleep();
    bmp.setSampling();
    delay(500);
    Serial.println("Going to sleep now");
    esp_deep_sleep_start();
}
/* ------------------ Method to print the reason by which ESP32 has been awaken from sleep */
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP:      Serial.println("Wakeup caused by ULP program"); break;
    default:                        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}
