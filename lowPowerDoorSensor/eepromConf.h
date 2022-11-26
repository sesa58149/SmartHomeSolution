#ifndef __eepromConf__
#define __eepromConf__

//#include <ESP_EEPROM.h>
#include <stdio.h>
#include <string.h>
#include <Arduino.h>

#define MAX_STR_LEN 28

typedef struct{
  char valid;
  char name[MAX_STR_LEN];
  char ssid[MAX_STR_LEN];
  char pass[MAX_STR_LEN];
}eepromConf;
class deviceConf
{
  protected:
  eepromConf devConf;
    
  public:
  deviceConf(){    
    memset((void*)&devConf, 0, sizeof(eepromConf)); 
    //EEPROM.begin(sizeof(eepromConf));  
  }
  int getDevConf(eepromConf* rConf);
  int setDevConf(eepromConf *dConf);
  int saveOnEeprom();
  int readFromEeprom(deviceConf* dconf, int );
  int setDefaultValue();
};

#endif //__eepromConf__