/**
 * @file LoraNode.h
 * @author Anders Boghammar
 * @brief JSon based protocol for LORA sensor nodes
 * @version 0.1
 * @date 2025-02-04
 * 
 * @copyright Copyright (c) boghammar.com 2025
 *
 */

#ifndef LORANODE_H
#define LORANODE_H

#include <LoraMessage.h>
#include <ArduinoJson.h>

// --------------------------------------------------------------------
// --------------------------------------------------------------------
class LoraNode {
public:

    LoraNode() {};

    void init(int sourceAddress) { _sourceAddress = sourceAddress; };

    void helloMessage(int destinationAddress, Device device);
    void ackMessage(int destinationAddress, long messageId, String ack, String error, String command);
    void configMessage(int destinationAddress, int sleepTime, int heartbeatFrequency, float deltaTemperature, float deltaHumidity, float deltaPressure);
    void reportMessage(int destinationAddress, Device device, int numSensors, SensorStatus sensorStatus[]);
    void dataMessage(int destinationAddress, int numSensors, SensorReading readings[] );

    LoraMessage getMessage(String json);

    String toString();

private:
    int _sourceAddress;
    JsonDocument _message;
    long _messageId = 0;
    
    long getMessageId();
    JsonObject initMessage(int destinationAddress, String type);

    LoraMessage createHelloMessage(JsonDocument jsondoc);
    LoraMessage createAckMessage(   JsonDocument jsondoc);
    LoraMessage createReportMessage(JsonDocument jsondoc);
    LoraMessage createConfigMessage(JsonDocument jsondoc);
    LoraMessage createDataMessage(  JsonDocument jsondoc);
};

#endif