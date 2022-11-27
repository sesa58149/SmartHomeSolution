#include"eepromConf.h"

int deviceConf::getDevConf(eepromConf* rConf)
{
  EEPROM.get(0, devConf);
  if(devConf.valid != 0xA5)
  {
    Serial.println("not a valid configuration");
    setDefaultValue();
  }
  memcpy((void*)rConf, (void*)&devConf, sizeof(eepromConf));
  return 0;
}
int deviceConf::setDevConf (eepromConf *dConf)
{
  memcpy( (void*)&devConf, (void*)dConf, sizeof(eepromConf));
  saveOnEeprom();
  return 0;
}

int deviceConf::saveOnEeprom()
{
  EEPROM.put(0, devConf);
  bool ok = EEPROM.commit();
  Serial.println((ok) ? "Second commit OK" : "Commit failed");
  return 0;
}

int deviceConf:: setDefaultValue()
{
  devConf.valid = 0xFF;
  memcpy((void*)&devConf.name, (void*)"KSHome_sensor", sizeof("KSHome_sensor"));
  memcpy((void*)&devConf.ssid, (void*)"subhSpec",sizeof("subhSpec"));
  memcpy((void*)&devConf.pass,(void*)"pawan@158",sizeof("pawan@158")); 
  memcpy( (void*)&devConf.mqttBrokerinfo.mqttBrokerAdd, "192.168.1.50", sizeof("192.168.1.50") );
  devConf.mqttBrokerinfo.mqttBrokerPort = 1883; 
  return 0;
}