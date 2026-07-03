# General idea

- **LORA nodes**:
  - **WaterTempNode**: A LORA based device with a temp sensor is attached near the water.
    - Battery driven
    - One temp sensors for measuring water temp. DS18B20 water proof and air proof
    - Initially try with ~~TTGO LORA32 but this might be too batteryhungry so if possible~~ a RAK4631 and a RAK19007 base board. Other alternatives are a design based on an ATTiny (or) PIC µP
  - **AIR&DisplayNode**: A LORA based device with temp and humidity sensor.
    - Solarpowered battery driven
    - Potentially we can use the TTGO LORA32 if we have a good solar panel that can power the batteries
    - It has a BME280 or BMP280 to measure air temperature and air pressure
    - ...
  - **LORAGateway**: A LORA based device functioning as a gateway between the LORA network and internet. 
    - AC/USB power alternativly Solarbattery
    - Connects to any available WiFi at the LORA node site.
    - Sensordata published to the backen MQTT Broker
    - Historical data back to nodes stored o retrived?
    - Based on a Heltec Wireless Stick V3
    - ...

- **Backend**
  - MQTT Broker: Container deployment of Mosquitto on idefix. Exposed via reverse proxy to the internet. Secured.
  - Automation: Container deployment of Node-Red on idefix. Only reachable within internal network.
  - Database: Container deployment of Influx on idefix. Only reachable within internal network.

- **Frontend**
  - "*SummerTime*": NodeJS Express application deployed on Idefix. Exposed via reverse proxy to the internet. Secured.
    - Start page shows current values watertemp, airtemp, humidity. Date and time of measuremenst. Current location and date and time
    - Possibility to show history of a sensor value.
    - Feedback form

![Design scetch](BadTemperature.drawio.png)

## InfluxDB

## MQTT setup

- Base topic is iot
- site and location are subtopics

MQTT is also used as a node monitoring using the birth and LWT messages.

# LORA devices
## Protocol

The LORA devices communicate with a JSon based protocol as payload.

see [LORAProtocol.md](./LORAProtocol.md)
## WaterTempNode

 ~~[Lilygo TTGO LORA32 V2.1](https://lilygo.cc/products/lora3?srsltid=AfmBOop02ZAt2DNnjyeRxOZtab0ygGJ0dLK-7sXWsq4PshTEKc9ZP_9H).~~
 
 [RAK4631](https://docs.rakwireless.com/product-categories/wisblock/rak4631/overview/)

Features:
- LoRa capable
- OneWire for the temp measurements
- Battery powered
- Deep sleep sending measurements every 30 minutes

## AIR&DisplayNode

Features:
- LoRA capable
- Battery powered with solar power support
- Deep sleep sending measurements every 30 minutes
- A paper display mounted outside that displays all data gathered by the system 


## LORAGateway

[Heltec Wireless Stick V3](https://heltec.org/project/wireless-stick-v3/).

Features:
- WiFi gateway for all LoRa nodes talking to the backend
- ...
