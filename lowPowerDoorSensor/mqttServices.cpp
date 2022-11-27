#include "mqttServices.h"

bool mqttServices::startMqttServices( char *clientId)
{
  return mqttClient.connect(clientId);
}

bool mqttServices::sendMessage( char* topic, char* message)
{
  return mqttClient.publish(topic, message);
}

bool mqttServices:: subscribeMessage(char* topic, int qos) 
{
  return mqttClient.subscribe(topic,qos);
}