 
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

#define Relay1            D0
#define Relay2            D1
#define Relay3            D2

char auth[] = "19ce6900c64140cd8bdc9063a5bb9884";

#define WLAN_SSID       "Deena's Tablet"             // Your SSID
#define WLAN_PASS       "deenukatab"        // Your password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "ls8835099"            // Replace it with your username
#define AIO_KEY         "a621f61bfd2e4b1f84aaf68f4bd93409"   // Replace with your Project Auth Key

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/


// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/light"); // FeedName
Adafruit_MQTT_Subscribe fan1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/fan"); // FeedName
Adafruit_MQTT_Subscribe switch1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/switch"); // FeedName

void MQTT_connect();

void setup() {
  Serial.begin(115200);

  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
 

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&Light1);
  mqtt.subscribe(&fan1);
  mqtt.subscribe(&switch1);

  Blynk.begin(auth, WLAN_SSID, WLAN_PASS);
}

void loop() {

  Blynk.run(); 
 
  MQTT_connect();
  
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &Light1) {
      Serial.print(F("Got: "));
      Serial.println((char *)Light1.lastread);
      int Light1_State = atoi((char *)Light1.lastread);
      digitalWrite(Relay1, !(Light1_State));
      
    }
    if (subscription == &switch1) {
      Serial.print(F("Got: "));
      Serial.println((char *)fan1.lastread);
      int switch1_State = atoi((char *)fan1.lastread);
      digitalWrite(Relay2, !(switch1_State));
      
    }
    if (subscription == &fan1) {
      Serial.print(F("Got: "));
      Serial.println((char *)fan1.lastread);
      int fan1_State = atoi((char *)fan1.lastread);
      digitalWrite(Relay3, !(fan1_State));
      
    }
  }
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  
}
