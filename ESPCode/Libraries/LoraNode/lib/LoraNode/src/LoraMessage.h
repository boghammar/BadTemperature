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

#ifndef LORAMESSAGE_H
#define LORAMESSAGE_H

#include <ArduinoJson.h>

#define MAX_SENSORS 8
// --------------------------------------------------------------------
// Enumerations used in the protocol
// --------------------------------------------------------------------
enum class LoraNodeDataUnits {
    CELCIUS = 1,
    FAHRENHEIT,
    KELVIN,
    VOLT,
    AMPERE,
    WATT,
    KWH,
    LUX,
    PERCENT,
    PASCAL,
    MILLIMETER,
    UNKNOWN
};
enum class LoraNodeDataTypes {
    TEMPERATURE = 1,
    HUMIDITY,
    PRESSURE,
    LIGHT,
    SOIL_MOISTURE,
    VOLTAGE,
    CURRENT,
    POWER,
    WIND_SPEED,
    WIND_DIRECTION,
    RAIN_INTENSITY,
    RAIN_DURATION,
    RAIN_DIRECTION,
    UNKNOWN,
}; 
// --------------------------------------------------------------------
// Classes used in the protocol
// -------------------------------------------------------------------
// --------------------------------------------------------- For I & R
class Device {
public: 
    String deviceName = "";
    String deviceType = "";
    String siteName = "";
    String locationName = ""; 
    int batteryLevel = -1;      /* Level in %. Unknown if -1 */ 
};

// --------------------------------------------------------- For R
class SensorStatus {
public: 
    int id;
    String location;
    String name;
    LoraNodeDataTypes valueType;
    LoraNodeDataUnits sensorUnit;
    float resolution;
    float accuracy;
};

// --------------------------------------------------------- For D
class SensorReading {
public: 
    int id;
    LoraNodeDataTypes dataType;
    float value;
};

// --------------------------------------------------------- Base
class LoraMessage {
public: 
    int messageId = -1;
    int sourceAddress = -1;
    int destinationAddress = -1;
    String messageType = ""; 

    virtual void set(JsonDocument jsondoc);
}; 
// --------------------------------------------------------- Hello
class HelloMessage : public LoraMessage {
public: 
    Device device;

    void set(JsonDocument jsondoc);
}; 
// --------------------------------------------------------- Ack
class AckMessage : public LoraMessage {
public: 
    long messageId = -1;
    String ack = ""; 
    String error = ""; 
    String command = "";

    void set(JsonDocument jsondoc);
};
// --------------------------------------------------------- Config
class ConfigMessage : public LoraMessage {
public: 
    int sleepTime = -1;
    int heartbeatFrequency = -1;
    float deltaTemperature = -1;
    float deltaHumidity = -1;
    float deltaPressure = -1;

    void set(JsonDocument jsondoc);
};
// --------------------------------------------------------- Report
class ReportMessage : public LoraMessage {
public: 
    Device device;
    SensorStatus sensors[MAX_SENSORS];

    void set(JsonDocument jsondoc);
};
// --------------------------------------------------------- Data
class DataMessage : public LoraMessage {
public: 
    SensorReading readings[MAX_SENSORS]; // Max 8 sensors per device

    void set(JsonDocument jsondoc);
};

#endif