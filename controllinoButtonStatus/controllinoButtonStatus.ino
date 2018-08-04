#include <Controllino.h>

//MQTT
#include <Ethernet.h>
#include <MQTT.h>

int buttonPinNumber = CONTROLLINO_R0;
int lightPinNumber = CONTROLLINO_A0;

EthernetClient net;
MQTTClient client;
byte mac[] = {0x38, 0xE6, 0xAD, 0x5D, 0x5D, 0x49};

int lightStatus = LOW;
String lightCommandTopic = "/house/light/command";
String lightStatusTopic = "/house/light/status";

void setup() {
  pinMode(buttonPinNumber, OUTPUT);
  pinMode(lightPinNumber, INPUT);
  Serial.begin(9600);

  Ethernet.begin(mac);
  client.begin("192.168.1.123", net);
  client.onMessage(handleMqttMessage);

  connect();
}


void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }
  
  int currentStatus = digitalRead(lightPinNumber);
  //Serial.println(analogRead(lightPinNumber));
  if(currentStatus != lightStatus) {
    lightStatus = currentStatus;
    if(lightStatus == HIGH) {
      Serial.println("La luce è accesa");
      client.publish(lightStatusTopic, "Light is on");
    } else {
      Serial.println("La luce è spenta");
      client.publish(lightStatusTopic, "Light is off");
    }
    // avoid noisy signal from the light relay 
    delay(300);
  }
  handleSerial();
}

void connect() {
  Serial.print("connecting...");
  while (!client.connect("","","")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe(lightCommandTopic);
  client.subscribe(lightStatusTopic);
  // client.unsubscribe(houseTopic);
}

void handleMqttMessage(String &topic, String &payload) {
  Serial.println("MQTT incoming message: " + topic + " - " + payload);
  if(topic == lightCommandTopic) {
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
  }
}

void sendImpulse(int pinNumber) {
  digitalWrite(pinNumber, HIGH);
  delay(25);
  digitalWrite(pinNumber, LOW);
}

