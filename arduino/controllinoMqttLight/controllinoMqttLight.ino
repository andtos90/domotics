#include <Controllino.h>
#include <EEPROM.h>

//MQTT
#include <Ethernet.h>
#include <PubSubClient.h>

EthernetClient net;
PubSubClient client(net);
byte mac[] = {0x38, 0xE6, 0xAD, 0x5D, 0x5D, 0x49};


struct Light {
  int buttonPinNumber;
  int lightPinNumber;
  int status;
  char name[12];
};

const short numberOfLights = 3;
Light lights[numberOfLights]= {
  {CONTROLLINO_R0, CONTROLLINO_A0, LOW, "living"},
  {CONTROLLINO_R1, CONTROLLINO_A1, LOW, "kitchen"},
  {CONTROLLINO_R2, CONTROLLINO_A2, LOW, "driveway"}
};

struct Cover {
  int openPinNumber;
  int closePinNumber;
  bool isOpening;
  bool isClosing;
  unsigned long movementStartTime;
  unsigned long movementDuration; //ms
  char name[12];
};

const short numberOfCovers = 2;
Cover covers[numberOfCovers]= {
  {CONTROLLINO_R7, CONTROLLINO_R6, false, false, 0, 30000, "first"},
  {CONTROLLINO_R8, CONTROLLINO_R9, false, false, 0, 30000, "ground"}
};

struct BinarySensor {
  int statusPinNumber;
  int status;
  char name[12];
};

const short numberOfBinarySensors = 1;
BinarySensor binarySensors[numberOfBinarySensors]= {
  {CONTROLLINO_A9, LOW, "gate"},
};


// Handle secret data with EEPROM
struct MQTTCredentials {
  char clientId[20];
  char serverIp[16];
  int serverPort;
  char user[20];
  char password[30];
};
MQTTCredentials credentials;

String getLightCommandTopic(Light* light) {
  return "/house/light/" + String(light->name) + "/command";
}

String getLightStatusTopic(Light* light) {
  return "/house/light/" + String(light->name) + "/status";
}

String getCoverCommandTopic(Cover* cover) {
  return "/house/cover/" + String(cover->name) + "/set";
}

String getBinarySensorStatusTopic(BinarySensor* sensor) {
  return "/house/binary/" + String(sensor->name) + "/status";
}

void setup() {
  // Setup pins and serial port
  for(short i=0; i<numberOfLights; i++) {
    pinMode(lights[i].buttonPinNumber, OUTPUT);
    pinMode(lights[i].lightPinNumber, INPUT);
  }
  for(short i=0; i<numberOfCovers; i++) {
    pinMode(covers[i].openPinNumber, OUTPUT);
    pinMode(covers[i].closePinNumber, OUTPUT);
  }
  for(short i=0; i<numberOfBinarySensors; i++) {
    pinMode(binarySensors[i].statusPinNumber, INPUT);
  }
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

void checkLightStatus(Light* light) {
  int currentStatus = digitalRead(light->lightPinNumber);
  if(currentStatus != light->status) {
    light->status = currentStatus;
    publishLightStatus(light);
  }
}

void checkCoverStatus(Cover* cover) {
  if(cover->isOpening || cover->isClosing) {
    unsigned long now = millis();        
    unsigned long elapsed = now - cover->movementStartTime;  
    if (elapsed >= cover->movementDuration){
      stopCoverMovement(cover);
    }
  }
}

void checkBinarySensorStatus(BinarySensor* sensor) {
  int currentStatus = digitalRead(sensor->statusPinNumber);
  if(currentStatus != sensor->status) {
    sensor->status = currentStatus;
    publishBinarySensorStatus(sensor);
  }
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  // Code below is exectued only when we're connected to the MQTT server
  client.loop();

  // Check status
  for(short i=0; i<numberOfLights; i++) {
    checkLightStatus(&lights[i]);
  }
  for(short i=0; i<numberOfCovers; i++) {
    checkCoverStatus(&covers[i]);
  }
  for(short i=0; i<numberOfBinarySensors; i++) {
    checkBinarySensorStatus(&binarySensors[i]);
  }
  // avoid noisy signal from the light relay 
  delay(100);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(credentials.clientId, credentials.user, credentials.password)) {
      Serial.println("INFO: connected");
      // Once connected, publish an announcement...
      for(short i=0; i<numberOfLights; i++) {
        publishLightStatus(&lights[i]);
      }
      for(short i=0; i<numberOfBinarySensors; i++) {
        publishBinarySensorStatus(&binarySensors[i]);
      }
      // ... and resubscribe
      for(short i=0; i<numberOfLights; i++) {
        char charTopic[30];
        String topic = getLightCommandTopic(&lights[i]);
        // TODO: avoid cast
        topic.toCharArray(charTopic, 30);
        client.subscribe(charTopic);
      }
      for(short i=0; i<numberOfCovers; i++) {
        char charTopic[30];
        String topic = getCoverCommandTopic(&covers[i]);
        // TODO: avoid cast
        topic.toCharArray(charTopic, 30);
        client.subscribe(charTopic);
      }
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void publishLightStatus(Light* light) {
  char charTopic[30];
  String topic = getLightStatusTopic(light);
  // TODO: avoid cast
  topic.toCharArray(charTopic, 30);
  if(light->status == HIGH) {
    Serial.println("Light is on");
    client.publish(charTopic, "ON", true);
  } else {
    Serial.println("Light is off");
    client.publish(charTopic, "OFF", false);
  }
}

void publishBinarySensorStatus(BinarySensor* sensor) {
  char charTopic[30];
  String topic = getBinarySensorStatusTopic(sensor);
  // TODO: avoid cast
  topic.toCharArray(charTopic, 30);
  if(sensor->status == HIGH) {
    Serial.println("Sensor is on");
    client.publish(charTopic, "ON", true);
  } else {
    Serial.println("Sensor is off");
    client.publish(charTopic, "OFF", false);
  }
}


void handleMqttMessage(char* topic, byte* p_payload, unsigned int p_length) {
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  bool found = false;
  // lights
  for(short i=0; i<numberOfLights && !found; i++) {
    String lightTopic = getLightCommandTopic(&lights[i]);
    if(String(lightTopic).equals(topic)) {
      handleLightMessage(&lights[i], payload);
      found = true;
    }
  }
  // covers
  for(short i=0; i<numberOfCovers && !found; i++) {
    String coverTopic = getCoverCommandTopic(&covers[i]);
    if(String(coverTopic).equals(topic)) {
      handleCoverMessage(&covers[i], payload);
      found = true;
    }
  }
}

void handleSerial() {
 while (Serial.available() > 0) {
   char incomingCharacter = Serial.read();
   handleLightMessage(&lights[0], String(incomingCharacter));
 }
}

void handleLightMessage(Light* light, String msg) {
  if(msg == "toggleLight") {
    sendImpulse(light->buttonPinNumber);
  } else if (msg == "ON") {
    if(light->status == LOW) sendImpulse(light->buttonPinNumber);
  } else if (msg == "OFF") {
    if(light->status == HIGH) sendImpulse(light->buttonPinNumber);
  }
}

void stopCoverMovement(Cover* cover) {
  Serial.println("STOP cover");
  digitalWrite(cover->openPinNumber, LOW);
  digitalWrite(cover->closePinNumber, LOW);
  cover->isOpening = false;
  cover->isClosing = false;
}

void handleCoverMessage(Cover* cover, String msg) {
  
  delay(1000);
  if(msg == "STOP") {
    Serial.println("STOP");
    stopCoverMovement(cover);
  } else if (msg == "OPEN") {
    if(!cover->isOpening) {
      Serial.println("OPEN");
      stopCoverMovement(cover);
      delay(1000);
      cover->movementStartTime = millis();
      cover->isOpening = true;
      digitalWrite(cover->openPinNumber, HIGH);
    }
  } else if (msg == "CLOSE") {
    if(!cover->isClosing) {
      Serial.println("CLOSE");
      stopCoverMovement(cover);
      delay(1000);
      cover->movementStartTime = millis();
      cover->isClosing = true;
      digitalWrite(cover->closePinNumber, HIGH);
    }
  }
}

void sendImpulse(int pinNumber) {
  digitalWrite(pinNumber, HIGH);
  delay(25);
  digitalWrite(pinNumber, LOW);
}

