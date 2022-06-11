#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "KHomeMqttClient.h"

#define mqttBroker  "192.168.1.50"
#define MQTT_PORT   1883


WiFiClient espWiFiClient;
PubSubClient pubSubClient(espWiFiClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void msgRcevCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void pubSubInit()
{
  pubSubClient.setServer(mqttBroker,MQTT_PORT);
  pubSubClient.setCallback(msgRcevCallback);
}

void reconnect() {
  // Loop until we're reconnected
  while (!pubSubClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (pubSubClient.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      pubSubClient.publish(pubTopic, "hello world");
      // ... and resubscribe
      pubSubClient.subscribe(subTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(pubSubClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void pubSubTaskloop()
{
  
  if (!pubSubClient.connected()) {
    reconnect();
  }
  pubSubClient.loop();
/*
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    pubSubClient.publish(pubTopic, msg);
  }
  */
}

void publishTopic(char * topic, char * msg)
{
  pubSubClient.publish(topic, msg);
}

