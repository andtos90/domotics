#include <Controllino.h>
#include <EEPROM.h>

//MQTT
#include <Ethernet.h>
#include <PubSubClient.h>

EthernetClient net;
PubSubClient client(net);
byte mac[] = {0x38, 0xE6, 0xAD, 0x5D, 0x5D, 0x49};


// TODO: support multiples lights and buttons
int buttonPinNumber = CONTROLLINO_R0;
int lightPinNumber = CONTROLLINO_A0;
int lightStatus = LOW;

// Handle secret data with EEPROM
struct MQTTCredentials {
  char clientId[20];
  char serverIp[16];
  int serverPort;
  char user[20];
  char password[30];
};
MQTTCredentials credentials;

// MQTT topics
const char* lightCommandTopic = "/house/light/command";
const char* lightStatusTopic = "/house/light/status";

void setup() {
  // Setup pins and serial port
  pinMode(buttonPinNumber, OUTPUT);
  pinMode(lightPinNumber, INPUT);
  Serial.begin(9600);

  // Read MQTT configuration from EEPROM
  EEPROM.get(0, credentials);
  Serial.println("Configuration read with success");

  // Debug configuration by writing server ip and port
  Serial.print("Server address: ");
  Serial.print(credentials.serverIp);
  Serial.print(":");
  Serial.println(credentials.serverPort);

  // Setup the MQTT connection (not connecting yet)
  Ethernet.begin(mac);
  client.setServer(credentials.serverIp, credentials.serverPort);
  client.setCallback(handleMqttMessage);
}


void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  // Code below is exectued only when we're connected to the MQTT server
  client.loop();

  // Check light status
  int currentStatus = digitalRead(lightPinNumber);
  if(currentStatus != lightStatus) {
    lightStatus = currentStatus;
    publishLightStatus();
    // avoid noisy signal from the light relay 
    delay(300);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(credentials.clientId, credentials.user, credentials.password)) {
      Serial.println("INFO: connected");
      // Once connected, publish an announcement...
      publishLightStatus();
      // ... and resubscribe
      client.subscribe(lightCommandTopic);
      client.subscribe(lightStatusTopic);
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void publishLightStatus() {
  if(lightStatus == HIGH) {
    Serial.println("Light is on");
    client.publish(lightStatusTopic, "ON", true);
  } else {
    Serial.println("Light is off");
    client.publish(lightStatusTopic, "OFF", false);
  }
}


void handleMqttMessage(char* topic, byte* p_payload, unsigned int p_length) {
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  if(String(lightCommandTopic).equals(topic)) {
    handleMessage(payload);
  }
}

void handleSerial() {
 while (Serial.available() > 0) {
   char incomingCharacter = Serial.read();
   handleMessage(String(incomingCharacter));
   
 }
}

void handleMessage(String msg) {
  if(msg == "toggleLight") {
    sendImpulse(buttonPinNumber);
  } else if (msg == "ON") {
    if(lightStatus == LOW) sendImpulse(buttonPinNumber);
  } else if (msg == "OFF") {
    if(lightStatus == HIGH) sendImpulse(buttonPinNumber);
  }
}

void sendImpulse(int pinNumber) {
  digitalWrite(pinNumber, HIGH);
  delay(25);
  digitalWrite(pinNumber, LOW);
}

