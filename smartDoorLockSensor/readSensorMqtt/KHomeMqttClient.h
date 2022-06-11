#ifndef _KHOME_MQTT_CLIENT_H__
#define _KHOME_MQTT_CLIENT_H__
#include "userDef.h"
  void pubSubTaskloop();
  void pubSubInit();
  void publishTopic(char* topic, char * msg);

  
#define subTopic "DoorSensor/MainDoor/cmd"
#define pubTopic "DoorSensor/MainDoor"
#define pubTopic1 "DoorSensor/MainDoor/Battery"

#endif //_KHOME_MQTT_CLIENT_H__
  
