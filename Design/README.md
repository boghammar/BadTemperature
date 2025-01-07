# General idea

- **LORA nodes**:
  - **WaterTempNode**: A LORA based device with a temp sensor is attached near the water.
    - Battery driven
    - ...
  _ **AIR&DisplayNode**: A LORA based device with temp and humidity sensor.
    - Solarpowered battery driven
    - ...
  - **LORAGateway**: A LORA based device functioning as a gateway between the LORA network and internet. 
    - AC/USB power alternativly Solarbattery
    - ...

# LORA Protocol

The LORA devices communicate with a JSon based protocol as payload.
There are 2 types of messages sensordata and configuration
A message can be broadcasted to all nodes or be directed to one node
All messages shall be acknowledged

```
{
   "mid" : "message id",
   "dst" : "destaddress Or Broadcast",
   "cmd" : "type of message"
   "msg" : "actual message"
}
```