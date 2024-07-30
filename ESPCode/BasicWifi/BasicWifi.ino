//#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>
#include <PubSubClient.h>

const char* ssid = "fullrulle";
const char* password = "0123456789";
const char* mqtt_server = "test.mosquitto.org"; //"broker.mqtt-dashboard.com";

String serverURL = "http://192.168.2.90:3000";

unsigned long previousBlink = 0;
const long blinkIntervall = 4000; 

ESP8266WiFiMulti WiFiMulti;
// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 5000;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ---------------------------------------------------------------------------------
// Utility functions declarations
// ---------------------------------------------------------------------------------
void StartWifi();
void PostSignal();
void GetStatus();
void MQTTMessageReceived(char* topic, byte* payload, unsigned int length);
void MQTTReconnect();

// ---------------------------------------------------------------------------------
// My Setup
// ---------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("");
  
  pinMode(LED_BUILTIN, OUTPUT);

  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(MQTTMessageReceived);
  mqttClient.setKeepAlive(20); // Default 15

  StartWifi();
}

// ---------------------------------------------------------------------------------
// My Loop
// ---------------------------------------------------------------------------------
void loop() {

  if (!mqttClient.connected()) {
    MQTTReconnect();
  }
  mqttClient.loop();

  // Do some LED blinking to see that it has been downloaded and works
  //
  unsigned long currentMillis = millis();

  // ----------------------------------- Do some blinking to see that we are alive
  if (currentMillis - previousBlink >= blinkIntervall) {
    mqttClient.publish("fullTopic/MyWorld","hello world");
    previousBlink = currentMillis;
    //delay(5000);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED on by making the voltage LOW
    delay(200);                      // wait for a 200 ms
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED off (HIGH is the voltage level)
    Serial.print(" SSID: ");
    Serial.print(WiFi.SSID());
    Serial.print(" RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {

      // -------------------------------------------- Do a post /signal
      PostSignal();
      // -------------------------------------------- Do a get /status
      GetStatus();
    } else {
      Serial.println("WiFi not connected");
    }
  }


}
// ---------------------------------------------------------------------------------
// Utility functions
// ---------------------------------------------------------------------------------
  void StartWifi() {
    // Don't save WiFi configuration in flash - optional
    WiFi.persistent(false);

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(ssid, password);
    //WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while (WiFiMulti.run(connectTimeoutMs) != WL_CONNECTED) {
    //while(WiFi.status() != WL_CONNECTED) { 
      Serial.printf(".%d",WiFi.status());
      delay(500);
    }
    Serial.println("");
    Serial.print("Connected to WiFi network as ");
    Serial.print(WiFi.hostname());
    Serial.print(" with IP Address: ");
    Serial.print(WiFi.localIP());
    Serial.print(" RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println();
  }
// ---------------------------------------------------------------------------------
void PostSignal() {
    HTTPClient http;
    String serverpath = serverURL + "/signal";
    Serial.printf("Doing a post for %s\r\n", serverpath.c_str());
    http.begin(wifiClient, serverpath.c_str());
    http.addHeader("Content-Type", "application/json");

    JSONVar jObj;
    jObj["host"] = WiFi.hostname();
    jObj["ssid"] = WiFi.SSID();
    jObj["rssi"] = WiFi.RSSI();
    Serial.printf("Payload %s\r\n", JSON.stringify(jObj).c_str());
    int httpCode = http.POST(JSON.stringify(jObj).c_str());

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}

// ---------------------------------------------------------------------------------
void GetStatus() {
      HTTPClient http;
      String serverpath = serverURL + "/status";
      Serial.printf("Doing a get for %s\r\n", serverpath.c_str());
      http.begin(wifiClient, serverpath.c_str());

      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("HTTP Response code (error): ");
        Serial.println(httpResponseCode);
      }
      
      http.end();
}
// ---------------------------------------------------------------------------------
void MQTTMessageReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// ---------------------------------------------------------------------------------
void MQTTReconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      mqttClient.publish("fullTopic","hello world");
      // ... and resubscribe
      mqttClient.subscribe("fullTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
