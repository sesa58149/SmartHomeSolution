#ifndef CONF_MGR_H_
#define CONF_MGR_H_

#include "arduino.h"
#include "ESP_EEPROM.h"

#include "slaveConf.h"

#include "userDef.h"

#define MAX_CONF_SIZE 128
#define CONF_START_ADDR 0
#define CONF_HEADER_SIZE 3

class configManager
{
  SLAVE_CONF deviceConf;
private:
  bool isConfEmpty(); 
  void writeDefaultConf();
 
public:
  configManager()
  {
    EEPROM.begin(MAX_CONF_SIZE);
    EEPROM.get(CONF_START_ADDR, deviceConf);
  }
  SLAVE_CONF* getDeviceConf();  
  WS_SINT32 putDeviceConf(SLAVE_CONF* conf);
 
};
#endif