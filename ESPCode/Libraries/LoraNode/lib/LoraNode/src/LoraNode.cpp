#include <LoraNode.h>

// --------------------------------------------------------------------
// PUBLIC API
// --------------------------------------------------------------------
void LoraNode::helloMessage(int destinationAddress, Device device) {
    JsonObject m = initMessage(destinationAddress, "H");

    m["n"] = device.deviceName;
    m["t"] = device.deviceType;
    m["s"] = device.siteName;
    m["l"] = device.locationName;
    m["b"] = device.batteryLevel;
}

void LoraNode::ackMessage(int destinationAddress, long messageId, String ack, String error, String command) {
    JsonObject m = initMessage(destinationAddress, "A");

    m["i"] = messageId;
    m["a"] = ack;
    m["e"] = error;
    m["c"] = command;
}

void LoraNode::configMessage(int destinationAddress, int sleepTime, int heartbeatFrequency, float deltaTemperature, float deltaHumidity, float deltaPressure) {
    JsonObject m = initMessage(destinationAddress, "C");

    m["s"] = sleepTime;
    m["h"] = heartbeatFrequency;
    m["dt"] = deltaTemperature;
    m["dh"] = deltaHumidity;
    m["dp"] = deltaPressure;
}

void LoraNode::reportMessage(int destinationAddress, Device device, int numSensors, SensorStatus sensors[]) {
    JsonObject m = initMessage(destinationAddress, "R");


    JsonObject d = m["d"].to<JsonObject>(); // Device status
    d["n"] = device.deviceName;
    d["t"] = device.deviceType;
    d["s"] = device.siteName;
    d["l"] = device.locationName;
    d["b"] = device.batteryLevel;

    JsonArray s = m["s"].to<JsonArray>(); // Sensor status array
    for (int i = 0; i < numSensors; i++) {
        JsonObject k = s[i].to<JsonObject>(); // Sensor status
        k["i"] = sensors[i].id;
        k["l"] = sensors[i].location;
        k["n"] = sensors[i].name;
        k["t"] = static_cast<int>(sensors[i].valueType);
        k["u"] = static_cast<int>(sensors[i].sensorUnit);
        k["r"] = sensors[i].resolution;
        k["a"] = sensors[i].accuracy;
    }
}


void LoraNode::dataMessage(int destinationAddress, int numSensors, SensorReading readings[]) {
    JsonObject m = initMessage(destinationAddress, "D");

    JsonArray s = m["s"].to<JsonArray>(); // Sensor status array
    for (int i = 0; i < numSensors; i++) {
        JsonObject k = s[i].to<JsonObject>(); // Sensor status
        k["i"] = readings[i].id;
        k["t"] = static_cast<int>(readings[i].dataType);
        k["v"] = readings[i].value;
    }
}

LoraMessage LoraNode::getMessage(String json) {
    DeserializationError error = deserializeJson(_message, json);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return LoraMessage();
    }
    Serial.println("Deser done mtype:");
    String mtype = _message["t"];
    Serial.println(mtype);
    switch (static_cast<char>(mtype.charAt(0))) {
        case 'H':
            return createHelloMessage(_message); 
            break;
        case 'A':
            return createAckMessage(_message); 
            break;
        case 'C':
            return createConfigMessage(_message); 
            break;
        case 'R':
            return createReportMessage(_message); 
            break;
        case 'D':
            return createDataMessage(_message); 
            break;
        default:
            return LoraMessage();
            break;
    }
}
// --------------------------------------------------------------------
String LoraNode::toString() {
    String out;
    serializeJson(_message, out);
    return out;
}

// --------------------------------------------------------------------
// PRIVATE METHODS
// --------------------------------------------------------------------

LoraMessage LoraNode::createHelloMessage( JsonDocument jsondoc) { HelloMessage msg;  msg.set(jsondoc);return msg;}
LoraMessage LoraNode::createAckMessage(   JsonDocument jsondoc) { AckMessage msg;    msg.set(jsondoc);return msg;}
LoraMessage LoraNode::createReportMessage(JsonDocument jsondoc) { ReportMessage msg; msg.set(jsondoc);return msg;}
LoraMessage LoraNode::createConfigMessage(JsonDocument jsondoc) { ConfigMessage msg; msg.set(jsondoc);return msg;}
LoraMessage LoraNode::createDataMessage(  JsonDocument jsondoc) { DataMessage msg;   msg.set(jsondoc);return msg;}

// --------------------------------------------------------------------
JsonObject LoraNode::initMessage(int destinationAddress, String type) {
    _message.clear();

    _message["i"] = getMessageId();
    _message["s"] = _sourceAddress;
    _message["d"] = destinationAddress;
    _message["t"] = type; 

    JsonObject m = _message["m"].to<JsonObject>();

    return m;
}

long LoraNode::getMessageId() {
    _messageId += 1;
    return _messageId;
}
