#include <LoraMessage.h>

void LoraMessage::set(JsonDocument jsondoc) {
    messageId = jsondoc["i"];
    destinationAddress = jsondoc["d"];
    sourceAddress = jsondoc["s"];
    messageType = String(jsondoc["t"].as<const char*>());
}

void HelloMessage::set(JsonDocument jsondoc) {
    LoraMessage::set(jsondoc);

    device.deviceName = String(jsondoc["m"]["n"].as<const char*>());
    device.deviceType = String(jsondoc["m"]["t"].as<const char*>());
    device.siteName = String(jsondoc["m"]["s"].as<const char*>());
    device.locationName = String(jsondoc["m"]["l"].as<const char*>());
    device.batteryLevel = jsondoc["m"]["b"];
}
void AckMessage::set(JsonDocument jsondoc) {
    LoraMessage::set(jsondoc);

    messageId = jsondoc["m"]["i"];
    ack = String(jsondoc["m"]["a"].as<const char*>());
    error = String(jsondoc["m"]["e"].as<const char*>());
    command = String(jsondoc["m"]["c"].as<const char*>());
}
void ConfigMessage::set(JsonDocument jsondoc) {
    LoraMessage::set(jsondoc);

    sleepTime = jsondoc["m"]["s"];
    heartbeatFrequency = jsondoc["m"]["h"];
    deltaTemperature = jsondoc["m"]["dt"];
    deltaHumidity = jsondoc["m"]["dh"];
    deltaPressure = jsondoc["m"]["dp"];
}
void ReportMessage::set(JsonDocument jsondoc) {
    LoraMessage::set(jsondoc);

    device.deviceName = String(jsondoc["m"]["n"].as<const char*>());
    device.deviceType = String(jsondoc["m"]["t"].as<const char*>());
    device.siteName = String(jsondoc["m"]["s"].as<const char*>());
    device.locationName = String(jsondoc["m"]["l"].as<const char*>());
    device.batteryLevel = jsondoc["m"]["b"];

    JsonArray s = jsondoc["m"]["s"].as<JsonArray>(); // Sensor status array
    int mxs = (s.size() > MAX_SENSORS ? MAX_SENSORS : s.size()); // TODO: Fix Silent ignore if more then MAX_SENSORS
    for (int i = 0; i < mxs; i++) {
        sensors[i].id = s[i]["i"];
        sensors[i].location = String(s[i]["l"].as<const char*>());
        sensors[i].name = String(s[i]["n"].as<const char*>());
        int vt = s[i]["t"];
        sensors[i].valueType = static_cast<LoraNodeDataTypes>(vt);
        int dt = s[i]["u"];
        sensors[i].sensorUnit = static_cast<LoraNodeDataUnits>(dt);
        sensors[i].resolution = s[i]["r"];
        sensors[i].accuracy = s[i]["a"];
    }
}
void DataMessage::set(JsonDocument jsondoc) {
    LoraMessage::set(jsondoc);

    JsonArray s = jsondoc["m"]["s"].as<JsonArray>(); // Sensor status array
    int mxs = (s.size() > MAX_SENSORS ? MAX_SENSORS : s.size()); // TODO: Fix Silent ignore if more then MAX_SENSORS
    for (int i = 0; i < mxs; i++) {
        readings[i].id = s[i]["i"];
        int dt = s[i]["t"];
        readings[i].dataType = static_cast<LoraNodeDataTypes>(dt);
        readings[i].value = s[i]["v"];
    }
}
