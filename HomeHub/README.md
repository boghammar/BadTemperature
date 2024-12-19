# HomeHub - Manage Shelly and Wiz devices
This is an fast and simple home automation hub that can manage Shelly and hopefully Wiz devices. Both device types are using Wifi commnication.

Automation is based on Node-RED

It is a node js app built around a basic Express app. 

## API

### backend

* /device?operation=*what* where *what* is :
  * **status**&id=*id* : obtain current status and other config data from the device with id=*id*
  * **refresh** : refresh status of all devices
  * **turnon**&id=*id*&turnon=*turnon* : sets the status of the device with id=*id* to on or off (*turnon* = true or false)

* /info?what=*whattodo* where *whattodo* is :
  * **hostname** : obtain the hostname of the server. Returns a string
  * **devices** : obtain the current devicelist held by the backend

### usage

script.js: when loading calls /info?what=devices 

