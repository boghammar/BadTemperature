#include <Arduino.h>
#include <LoraNode.h>

LoraNode message;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Device device;
  device.deviceName = "WaterTempNode";
  device.deviceType = "Lilygo TTGO LORA32";
  device.siteName = "norrtorp";
  device.locationName = "shore";
  device.batteryLevel = 50;

  SensorStatus sensorStatus[3];
  sensorStatus[0].id = 1;
  sensorStatus[0].location = "shore";
  sensorStatus[0].name = "BMP280";
  sensorStatus[0].valueType = LoraNodeDataTypes::TEMPERATURE;
  sensorStatus[0].sensorUnit = LoraNodeDataUnits::CELCIUS;
  sensorStatus[0].resolution = 0.1;
  sensorStatus[0].accuracy = 0.01;

  sensorStatus[1].id = 2;
  sensorStatus[1].location = "shore";
  sensorStatus[1].name = "BMP280";
  sensorStatus[1].valueType = LoraNodeDataTypes::HUMIDITY;
  sensorStatus[1].sensorUnit = LoraNodeDataUnits::PERCENT;
  sensorStatus[1].resolution = 0.1;
  sensorStatus[1].accuracy = 0.01;

  sensorStatus[2].id = 3;
  sensorStatus[2].location = "lake";
  sensorStatus[2].name = "DS18B20";
  sensorStatus[2].valueType = LoraNodeDataTypes::TEMPERATURE;
  sensorStatus[2].sensorUnit = LoraNodeDataUnits::CELCIUS;
  sensorStatus[2].resolution = 0.1;
  sensorStatus[2].accuracy = 0.01;

  message.init(2);

  message.helloMessage(1, device);
  Serial.print("HELLOMSG: ");
  String hjson = message.toString();
  Serial.println(hjson);

  message.ackMessage(1,2, "ACK", "ERROR", "COMMAND");
  Serial.print("ACKMSG: ");
  Serial.println(message.toString());

  message.configMessage(1, 10, 100, 1.1, 0.02, 1.3);
  Serial.print("CONFIGMSG: ");
  Serial.println(message.toString());

  message.reportMessage(1, device, 3, sensorStatus);
  Serial.print("REPORTMSG: ");
  Serial.println(message.toString());

  SensorReading readings[2];
  readings[0].id = 1;
  readings[0].dataType = LoraNodeDataTypes::TEMPERATURE;
  readings[0].value = 10.1;

  readings[1].id = 2;  
  readings[1].dataType = LoraNodeDataTypes::HUMIDITY;
  readings[1].value = 74.1;

  message.dataMessage(1, 2, readings);
  Serial.print("DATAMSG: ");
  String json = message.toString();
  Serial.println(json);
  try {
    LoraMessage msg = message.getMessage(json);

    Serial.printf("Device: i=%d, s=%d, d=%d, t=%s\n", msg.messageId, msg.sourceAddress, msg.destinationAddress, msg.messageType);
  } catch (std::exception ex) {
    Serial.println("Exception caught");
    Serial.printf("%s\n", ex.what());
  }
}

void loop() {
}

