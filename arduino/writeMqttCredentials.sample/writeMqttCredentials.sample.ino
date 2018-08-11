#include <Controllino.h>
#include <EEPROM.h>

// Handle secret data with EEPROM
struct MQTTCredentials {
  char clientId[20];
  char serverIp[16];
  int serverPort;
  char user[20];
  char password[30];
};

// Place your configuration here, more info here https://www.home-assistant.io/docs/mqtt/broker.
// This is a sample configuration for the HBMQTT broker
MQTTCredentials credentials = {
  "room_light", // Client Id (use what you want)
  "192.168.1.168", // MQTT broker ip address (homeassistant ip) 
  1883, // MQTT broker port 
  "homeassistant", // User (homeassistant is the default)
  "password" // Your api_password from homeassistant
};

void setup() {
  Serial.begin(9600);

  EEPROM.put(0, credentials);
  Serial.println("Configuration written with success");
}

void loop() {
}

