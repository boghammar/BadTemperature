/*
 * BadTemperature for the Heltec Wireless Stick V3
 *    based on the Heltec FactoryTest for this device
 *
 * boghammar.com 2025
 */
#include "Arduino.h"
#include "WiFi.h"
#include "LoRaWan_APP.h"
#include <Wire.h>  
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "./secrets.h"
#include "HT_SSD1306Wire.h"
#define WIFI_SSID "Ompabompa" // "Skistar" // "Anders iPhone 13" // 
#define WIFI_PASSWORD "Bonnie23" // "SkistarVemdalen1"  // "0123456789" // 

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

/********************************* lora  *********************************************/
#define RF_FREQUENCY                                868000000 // Hz
 
#define TX_OUTPUT_POWER                             14        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 100 // Define the payload size here

/********************************* lora declarations *********************************************/
static RadioEvents_t RadioEvents;

typedef enum {
    LOWPOWER,
    STATE_RX,
    STATE_TX
} States_t;

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

int16_t txNumber;
int16_t rxNumber;
States_t state;
bool sleepMode = false;
int16_t Rssi,rxSize;

String rssi = "RSSI --";
String packSize = "--";
String packet;
String lastValue = "";
String send_num;
String show_lora = "lora data show";

unsigned int counter = 0;

bool receiveflag = false; // software flag for LoRa receiver, received data makes it true.

long lastSendTime = 0;        // last send time
int interval = 1000;          // interval between sends
int16_t RssiDetection = 0;

// -------------------------------------------------------------------- 
// LORA handling
// -------------------------------------------------------------------- 
void OnTxDone( void ) {
  	Serial.print("TX done......");
	  state=STATE_RX;

}
// -------------------------------------------------------------------- 
void OnTxTimeout( void ) {
    Radio.Sleep( );
    Serial.print("TX Timeout......");
	  state=STATE_TX;
}
// -------------------------------------------------------------------- 
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ) {
	  rxNumber++;
    Rssi=rssi;
    rxSize=size;
    memcpy(rxpacket, payload, size );
    rxpacket[size]='\0';
    Radio.Sleep( );

    Serial.printf("\r\nreceived packet \"%s\" with Rssi %d , length %d\r\n",rxpacket,Rssi,rxSize);
    Serial.println("wait to send next packet");
	  receiveflag = true;
    state=STATE_TX;
}
// -------------------------------------------------------------------- 
void lora_init(void) {
    Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
    txNumber=0;
    Rssi=0;
	  rxNumber = 0;
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxDone = OnRxDone;

    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
	  
    // Starting state is transmitt
    state=STATE_TX;
}
// -------------------------------------------------------------------- 
void lora_send(void) {
	txNumber++;
	sprintf(txpacket,"hello %d, Rssi : %d",txNumber,Rssi);
	Serial.printf("\r\nsending packet \"%s\" , length %d\r\n",txpacket, strlen(txpacket));
	Radio.Send( (uint8_t *)txpacket, strlen(txpacket) );
}
// -------------------------------------------------------------------- 
// Display utils
// -------------------------------------------------------------------- 

SSD1306Wire  factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_64_32, RST_OLED); // addr , freq , i2c group , resolution , rst

// -------------------------------------------------------------------- 
// WiFi utils
// -------------------------------------------------------------------- 
void WIFISetUp(void) {
	// Set WiFi to station mode and disconnect from an AP if it was previously connected
	WiFi.disconnect(true);
	delay(100);
	WiFi.mode(WIFI_STA);
	WiFi.setAutoReconnect(true);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  
	delay(100);

	byte count = 0;
	while(WiFi.status() != WL_CONNECTED && count < 10) {
		count ++;
		delay(500);
		factory_display.drawString(0, 0, "Connecting...");
		factory_display.display();
	}
	factory_display.clear();

	if(WiFi.status() == WL_CONNECTED) {
		factory_display.drawString(0, 8, "OK.");
		factory_display.display();
    Serial.printf("Connected to '%s'", WIFI_SSID);
    
    wifiClient.setCACert(root_ca);
	} else {
		factory_display.clear();
		factory_display.drawString(0, 8, "Failed");
		factory_display.display();
    Serial.printf("Failed to connect to '%s'", WIFI_SSID);
	}

	factory_display.drawString(0, 16, "WIFI Setup done");
	factory_display.display();
	delay(500);
}

// -------------------------------------------------------------------- 
// GPIO0 (User switch) handling
// -------------------------------------------------------------------- 
bool resendflag=false;
bool deepsleepflag=false;
bool interrupt_flag = false;
// -------------------------------------------------------------------- 
void interrupt_GPIO0() {
	interrupt_flag = true;
}
// -------------------------------------------------------------------- 
void interrupt_handle(void) {
	if(interrupt_flag) {
		interrupt_flag = false;
		if(digitalRead(0)==0) {
			if(rxNumber <=2) { // If we have not receivde more then 2 messages => resend
				resendflag=true;
			} else {           // else we go to deepsleep
				deepsleepflag=true;
			}
		}
	}
}
// -------------------------------------------------------------------- 
// VEXT utils
// -------------------------------------------------------------------- 
void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
  
}

void VextOFF(void) //Vext default OFF
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}

// -------------------------------------------------------------------------------------- 
// -------------------------------------------------------------------- Standard setup
// -------------------------------------------------------------------------------------- 
void setup() {
  Serial.begin(115200);
  uint64_t chipid=ESP.getEfuseMac();    //The chip ID is essentially its MAC address(length: 6 bytes).
	Serial.printf("ESP32ChipID=%04X",(uint16_t)(chipid>>32));   //print High 2 bytes
	Serial.printf("%08X\n",(uint32_t)chipid);                   //print Low 4bytes.

	VextON();
	delay(100);
  lora_init();

  // ------------------------ Init display
	factory_display.init();
  factory_display.clear();
  factory_display.display();

  factory_display.drawString(0, 0, "Test Start");
	factory_display.display();
  factory_display.clear();

  Serial.println("Display initialized. Setting up WiFi");

  // ------------------------ Init WiFi
	WIFISetUp();
  factory_display.clear();

  // ------------------------ Init MQTT
  Serial.printf("Setting mqtt server %s on port %d\n",mqtt_server, mqtt_port);
  mqttClient.setServer(mqtt_server, mqtt_port);

  // Attach interupthandler to user switch (GPIO0)
  attachInterrupt(0,interrupt_GPIO0,FALLING);

  // ------------------------ Ready to go
	packet ="Setup done";
  factory_display.drawString(0, 0, packet);
  factory_display.display();
  delay(100);
  factory_display.clear();
	pinMode(LED ,OUTPUT);
	digitalWrite(LED, LOW);  

}

// -------------------------------------------------------------------------------------- 
// -------------------------------------------------------------------- Standard loop
// -------------------------------------------------------------------------------------- 
void loop() {
  // Check user switch has been pushed first
  interrupt_handle();

  if (deepsleepflag) { // --- Going to deepsleep
    Serial.println("Going to deepsleep");
    VextOFF();
    Radio.Sleep();
    pinMode(RADIO_DIO_1,ANALOG);
    pinMode(RADIO_NSS,ANALOG);
    pinMode(RADIO_RESET,ANALOG);
    pinMode(RADIO_BUSY,ANALOG);
    pinMode(LORA_CLK,ANALOG);
    pinMode(LORA_MISO,ANALOG);
    pinMode(LORA_MOSI,ANALOG);
    pinMode(LED,ANALOG);
    pinMode(Vext,ANALOG);
    detachInterrupt(0);
    Wire.end();
    Serial.end();//true);
    SPI.end();
    WiFi.mode(WIFI_OFF);
    esp_sleep_enable_timer_wakeup(600*1000*(uint64_t)1000); //Sleep for 600 seconds (10 min)
    esp_deep_sleep_start();
  }

  // ----------------------------- Process MQTT
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // ----------------------------- Display send/recv status
  if (receiveflag) {
    receiveflag = false; // signal that we have read incomming 
    packet = "R:";
    int i = 0;
    while(i < rxSize) {
      packet += rxpacket[i];
      i++;
    }
    packSize = "Rssi: ";
    packSize += String(Rssi,DEC);
    send_num = "S: ";
    send_num += String(txNumber,DEC);
	  factory_display.drawString(0, 0, "LoRa");
    factory_display.drawString(0, 10, packet);
    factory_display.drawString(0, 20, packSize);
    factory_display.drawString(32, 0, send_num);
    factory_display.display();
    delay(10);
    factory_display.clear();

    int len = packet.length();
    String value = packet.substring(len-1-5);
    if (!value.equals(lastValue)) {
      lastValue = value;
      publishMessage(value.c_str());
    }

  }
  // ----------------------------- Handle current state
  switch(state) {
      case STATE_TX:
        delay(1000);
        txNumber++;
        sprintf(txpacket,"hello %d,Rssi:%d",txNumber,Rssi);
        Serial.printf("\r\nsending packet \"%s\" , length %d\r\n",txpacket, strlen(txpacket));
        Radio.Send( (uint8_t *)txpacket, strlen(txpacket) );
        state=LOWPOWER;
        break;
      case STATE_RX:
        Serial.println("into RX mode");
        Radio.Rx( 0 );
        state=LOWPOWER;
        break;
      case LOWPOWER:
        Radio.IrqProcess( );
        break;
      default:
        break;
    }
}

// -------------------------------------------------------------------- 
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.printf("Attempting MQTT connection to %s on port %d ...",mqtt_server, mqtt_port);
    if (mqttClient.connect("HeltecClient", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(3000);
    }
  }
}
// -------------------------------------------------------------------- 
void publishMessage(const char* value) {
  if (mqttClient.connected()) {
    Serial.printf("Publishing to iot/norrtorp/shore/temp %s\n", value);
    mqttClient.publish("iot/norrtorp/shore/temp", value);
  }
}

