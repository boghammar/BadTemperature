/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com  
*********/

#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
// TTGO T3_V1.6.1
//      GPIO12 works but it cant be connected to the DS18B20 when flashing. I get an checksum error. Its ok to have a diod attached when flashing though
//      GPIO25 is builtin LED
//      GPIO34-36,39 are input only.
const int oneWireBus = 12; // TTGO GPIO12 // 4; //8266 GPIO4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

int numberOfDevices = 0;
DeviceAddress tempDeviceAddress;
// function to print a device address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  Serial.println();
  Serial.println("Temp tests -----------------------");
  // Start the DS18B20 sensor
  sensors.begin();
  delay(500);
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}

void loop() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  delay(4000);
}
