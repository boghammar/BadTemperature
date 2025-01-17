#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "../HeltecWirelessStick/HeltecWirelessStick/secrets.h"

const char* ssid = "Skistar";
const char* password = "SkistarVemdalen1";

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);


// -------------------------------------------------------------------- 
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.printf("Connected to WiFi %s\n",ssid);

  wifiClient.setCACert(root_ca);

/* -------------- Just testing HTTPS connection
  if (!wifiClient.connect(mqtt_server, mqtt_port)) {
    Serial.println("Connection failed");
    return;
  }
  Serial.println("Connected to server!");

    // Make a HTTP request:
    wifiClient.println("GET https://idefix.andersboghammar.com/info HTTP/1.0");
    wifiClient.println("Host: idefix.andersboghammar.com");
    wifiClient.println("Connection: close");
    wifiClient.println();

    while (wifiClient.connected()) {
      String line = wifiClient.readStringUntil('\n');
      Serial.println(line);
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
*/
  Serial.printf("Setting mqtt server %s on port %d\n",mqtt_server, mqtt_port);
  mqttClient.setServer(mqtt_server, mqtt_port);
}

// -------------------------------------------------------------------- 
void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  publishMessage();
  delay(5000);
}

// -------------------------------------------------------------------- 
void reconnect() {
  while (!mqttClient.connected()) {
    Serial.printf("Attempting MQTT connection to %s on port %d ...",mqtt_server, mqtt_port);
    if (mqttClient.connect("HeltecClient", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(3000);
    }
  }
}

// -------------------------------------------------------------------- 
void publishMessage() {
  if (mqttClient.connected()) {
    Serial.println("Publishing to iot/slask");
    mqttClient.publish("iot/slask", "Hello from Heltec!");
  }
}
