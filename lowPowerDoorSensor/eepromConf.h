#ifndef __eepromConf__
#define __eepromConf__


#include <stdio.h>
#include <string.h>
#include <Arduino.h>
#include <ESP_EEPROM.h>

#define MAX_STR_LEN 32

typedef struct{
  char mqttBrokerAdd[MAX_STR_LEN];
  int mqttBrokerPort;
}mqttBroker;

typedef struct{
  char valid;
  char name[MAX_STR_LEN];
  char ssid[MAX_STR_LEN];
  char pass[MAX_STR_LEN];
  mqttBroker mqttBrokerinfo;
}eepromConf;
class deviceConf
{
  protected:
  eepromConf devConf;
    
  public:
  deviceConf(){    
    memset((void*)&devConf, 0, sizeof(eepromConf)); 
    EEPROM.begin(sizeof(eepromConf));  
  }
  int getDevConf(eepromConf* rConf);
  int setDevConf(eepromConf *dConf);
  int saveOnEeprom();
  int setDefaultValue();
};

#endif //__eepromConf__