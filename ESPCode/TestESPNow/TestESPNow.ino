/*
 * Testing ESP NOW by setting up the same code on both devices and have them randomly send data to each other.
 *
 */
#include <ESP8266WiFi.h>

/* --------------------------------------------------------- Setup */
void setup() {
  // Print board MAC address
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  pinMode(LED_BUILTIN, OUTPUT);
}

/* --------------------------------------------------------- Main Loop */
void loop() {
  // Do some LED blinking to see that it has been downloaded and works
  //
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(2000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(200);                      // wait for a second
}
