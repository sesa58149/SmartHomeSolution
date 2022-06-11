#include "kshConfMgr.h"

bool configManager:: isConfEmpty()
  {
    bool retVal = true;
    if(deviceConf.validStr[0] == 'K' && deviceConf.validStr[1] == 'S' && deviceConf.validStr[2] == 'H')
    {
      retVal = false;      
    }
    return retVal; 
  }

  void configManager::writeDefaultConf()
  {
    deviceConf.validStr[0] = 'K';
    deviceConf.validStr[1] = 'S';
    deviceConf.validStr[2] = 'H';

    deviceConf.masterMac[0] = 0X64;
    deviceConf.masterMac[1] = 0X32;
    deviceConf.masterMac[2] = 0X10;
    deviceConf.masterMac[3] = 0X20;
    deviceConf.masterMac[4] = 0X30;
    deviceConf.masterMac[5] = 0X40;
    ws_memcpy(deviceConf.deviceName, DEFAULT_DEVICE_NAME, sizeof(DEFAULT_DEVICE_NAME));
    deviceConf.deviceType = 0xFF;
    deviceConf.wifiClass = 11;
    deviceConf.wifiType = SLAVE_STA;

    EEPROM.put(CONF_START_ADDR, deviceConf);
    if( EEPROM.commit() ==  true)
    {
      Serial.println(" deafult Configuration written successfully");
    }
    else
    {
      Serial.println(" deafult Configuration written failed");
    }

  }

  WS_SINT32 configManager::putDeviceConf(SLAVE_CONF* conf)
  {
    SLAVE_CONF tmpConf;
    ws_memcpy(&tmpConf, conf, sizeof(SLAVE_CONF));
    EEPROM.put(CONF_START_ADDR, deviceConf);
    if( EEPROM.commit() ==  true)
    {
     ws_memcpy(&deviceConf, conf, sizeof(SLAVE_CONF));
    }
    return WS_SUCCESS;
  }

  SLAVE_CONF* configManager::getDeviceConf()
  {
     if(isConfEmpty() == true)
     {
       writeDefaultConf();
       Serial.println(" empty conf");
     }
    
    return &deviceConf;
  }

