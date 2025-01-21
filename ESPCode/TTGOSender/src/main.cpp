/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
*********/

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Arduino.h"

//libraries for OneWire
#include <OneWire.h>
#include <DallasTemperature.h>

#include "loramessage.h"

// -------------------- Define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23 //14
#define DIO0 26

// -------------------- Define the LoRa settings
#define BAND 868000000 //866E6
#define TX_OUTPUT_POWER                             14        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

// -------------------- Define the pins used by the OLED pins
#define OLED_SDA 21 //4
#define OLED_SCL 22 //15 
#define OLED_RST -1 //16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// -------------------- Define the pins used by the OneWire pins
#define ONE_WIRE_BUS 12

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ---------------------------------- Function prototypes 

void onReceive(int packetSize);
void sendMessage(String message);
void sendErrorMsg(String error);
String getMsgId();

/* ------------------ define sleep parameters */
#define TIME_TO_SLEEP  10          /* Time ESP32 will go to sleep (in seconds) */
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
RTC_DATA_ATTR int bootCount = 0;   /* store data in RTC memory to use it over reboots*/
void print_wakeup_reason();

/* ------------------ define states */
#define STATE_SEND 0 
#define STATE_RECEIVE 1
#define STATE_GO_TO_SLEEP 2
int state = STATE_SEND;

/* ------------------ define variables */
int rssi = 0;
//packet counter
int counter = 0;
String msg  = "";
String lastInMsg = "";
float tempDiffThreshold = 0.8;                  // Only report if change is this much
RTC_DATA_ATTR float lastReportedTemperature = -127.00;  // Persistent beween reboots
RTC_DATA_ATTR int gMsgId = 0;                   // Persistent beween reboots
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// ---------------------------------------------------------------------------------------  Standard setup
void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while(!Serial);

  char ssid[23];
  snprintf(ssid, 23, "ESP-%llX", ESP.getEfuseMac());
  Serial.print("---------------------------------------------------------- LoRa with TTGO LoRa32 V2.1 ");
  Serial.println(ssid);

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason
  print_wakeup_reason();

  // ------------------ reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // ------------------ initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println("OLED init done");

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER Boot# ");
  display.print(bootCount);
  display.display();
  
  // ------------------ SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  // ------------------ setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // ------------------ configure LoRa settings
  // LoRa.setTxPower(TX_OUTPUT_POWER);
  // LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR);
  // LoRa.setSignalBandwidth(LORA_BANDWIDTH);
  // LoRa.setCodingRate4(LORA_CODINGRATE);
  // LoRa.setPreambleLength(LORA_PREAMBLE_LENGTH);
  // LoRa.setSymbolTimeout(LORA_SYMBOL_TIMEOUT);
  // LoRa.setFixLengthPayloadOn(LORA_FIX_LENGTH_PAYLOAD_ON);
  // LoRa.setIqInversion(LORA_IQ_INVERSION_ON);
  // ------------------ start listening for incomming packets
  LoRa.onReceive(onReceive);
  LoRa.receive();

  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.print("LoRa Initializing OK!");
  display.display();

  sensors.begin(); // Init the DS18B20 sensor
  delay(2000);
  int numberOfDevices = sensors.getDeviceCount();
  int numberOfSensors = sensors.getDS18Count();
  Serial.printf("DS18B20 sensor Initializing OK!\nDevices %d Sensors %d\n", numberOfDevices, numberOfSensors);
}

// --------------------------------------------------------------------------------------- Standard loop
void loop() {
 
  
  if (millis() - lastSendTime > interval && state == STATE_SEND) {  // check if it's time to send
    sensors.requestTemperatures(); 
    float temperatureC = sensors.getTempCByIndex(0);

    if (temperatureC != -127.00 && temperatureC != lastReportedTemperature) { // temperature is valid and has changed
      Serial.print(temperatureC);
      Serial.println("ºC");

      float diff = temperatureC - lastReportedTemperature;
      if (diff < 0) {
        diff = -diff;
      }
      Serial.print("Diff: ");
      Serial.println(diff);

      if (diff > tempDiffThreshold) {

        Serial.print("Sending packet: ");

        msg = "Loc " + String(counter) + " " + String(temperatureC);
        Serial.println(msg);
        sendMessage(msg);
        lastSendTime = millis();            // timestamp the message
        interval = random(2000) + 1000;     // 2-3 seconds
        LoRa.receive();                     // go back into receive mode
        counter++;
        lastReportedTemperature = temperatureC;

        state = STATE_RECEIVE;
        LoRa.receive();
      } else {
        Serial.printf("No change last %fºC curr %fºC diff %fºC\n", lastReportedTemperature, temperatureC, diff);
        state = STATE_GO_TO_SLEEP;
      }
    } else {
      if (temperatureC == -127.00) {
        Serial.println("No temp sensor(s) found");
        sendErrorMsg("No temp sensor(s) found");
        state = STATE_RECEIVE;
        LoRa.receive();
      } else {
        state = STATE_GO_TO_SLEEP;
      }
    }
  }

  // ------------------ display send/recv status  
  String tmp = "";
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("LORA TRANSCEIVER ST=");
  display.println(state);
  
  display.setCursor(0,20);
  display.setTextSize(1);
  tmp = "S: " + msg;
  display.print(tmp);
  
  display.setCursor(0,30);
  tmp = "R: " + lastInMsg;
  display.print(tmp);
  
  display.setCursor(0,40);
  display.print("Rssi:");
  display.print(rssi);
  
  display.display();

  if (state == STATE_GO_TO_SLEEP) {
    Serial.println("Going to sleep now");
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    LoRa.sleep();
    delay(500);
    esp_deep_sleep_start();
  }
  
  //delay(500);
}

// ----------------------------------------------------------------------------
// SLEEP FUNCTIONS
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// LORA FUNCTIONS
// ----------------------------------------------------------------------------
// ---------------------------------------------------------- Send a message
void sendMessage(String message) {
  String msgId = getMsgId();
  // Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
}
// ---------------------------------------------------------- Send an error message
void sendErrorMsg(String error) {

  String msgId = getMsgId();
  LoraMessage loraMsg(msgId, String(GATEWAY_ID), String(CMD_ERROR_REPORT), error);
  String json = loraMsg.getMessage();
  Serial.println(json);
  LoRa.beginPacket();
  LoRa.print(json);
  LoRa.endPacket();  
}
// ---------------------------------------------------------- Recieve callback
void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  // int recipient = LoRa.read();          // recipient address
  // byte sender = LoRa.read();            // sender address
  // byte incomingMsgId = LoRa.read();     // incoming msg ID
  // byte incomingLength = LoRa.read();    // incoming msg length

  lastInMsg = "";                 // payload of packet

  while (LoRa.available()) {            // can't use readString() in callback, so
    lastInMsg += (char)LoRa.read();      // add bytes one by one
  }

  rssi = LoRa.packetRssi();
  Serial.print("Received packet '");
  Serial.print(lastInMsg);
  Serial.print("' with RSSI ");
  Serial.println(rssi);
  // if (incomingLength != incoming.length()) {   // check length for error
  //   Serial.println("error: message length does not match length");
  //   return;                             // skip rest of function
  // }
  state = STATE_GO_TO_SLEEP;
}
// ---------------------------------------------------------- Get the messageID
String getMsgId() {
  String id = "ESP"+String(ESP.getEfuseMac())+String("_")+String(gMsgId);
  gMsgId++;
  return id;
}
