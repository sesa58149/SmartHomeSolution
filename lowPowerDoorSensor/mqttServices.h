#include "ESP8266WiFi.h"
#include "core_esp8266_features.h"
#include "WiFiClient.h"
#include "PubSubClient.h"
#include "eepromConf.h"

typedef void (*mqttSubCallBack) (char* topic, byte* payload, unsigned int length);


class mqttServices
{
  protected:
    int serviceInitialized;   
    PubSubClient mqttClient;
    WiFiClient wifiClient;   

  public:
  mqttServices(){
    serviceInitialized = 0;    
  }  
  mqttServices(mqttBroker* broker, mqttSubCallBack callback)
  {
    mqttClient.setClient(wifiClient);
    mqttClient.setServer(broker->mqttBrokerAdd, broker->mqttBrokerPort);
    mqttClient.setCallback(callback);
    serviceInitialized = 1;
  }

  bool startMqttServices( char *clientId);

  bool sendMessage( char* topic, char* message);

  bool subscribeMessage(char* topic, int qos);

  bool isConnected()
  {
    return mqttClient.connected();
  }

  void recvUnblock()
  {
    mqttClient.loop();
  }


};