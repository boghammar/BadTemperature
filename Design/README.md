# General idea

- **LORA nodes**:
  - **WaterTempNode**: A LORA based device with a temp sensor is attached near the water.
    - Battery driven
    - Initially try with TTGO LORA32 but this might be too batteryhungry so if possible design oe based on an ATTiny (or) PIC mp
  - **AIR&DisplayNode**: A LORA based device with temp and humidity sensor.
    - Solarpowered battery driven
    - ...
  - **LORAGateway**: A LORA based device functioning as a gateway between the LORA network and internet. 
    - AC/USB power alternativly Solarbattery
    - Sensordata published to MQTT Broker
    - Historical data back to nodes stored o retrived?
    - Based on a Heltec Wireless Stick V3
    - ...

- **Backend**
  - MQTT Broker: Container deployment of Mosquitto on idefix. Problem with exposing since router behind Nat potential alternativ use HiveMQ until Cloudflare steps up
  - Database: Container deployment of Influx on idefix.
  - Automation: Container deployment of Node-Red on idefix.

- **Frontend**
  - NodeJS Express application deployed on Idefix. 

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