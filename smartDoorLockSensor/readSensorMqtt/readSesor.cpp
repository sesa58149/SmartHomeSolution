#include "userDef.h"
#include "readSensor.h"
#include "KHomeMqttClient.h"

void sendNotificationReadSensorStatus()
{  
   
  publishTopic(pubTopic, "Open");
  
}