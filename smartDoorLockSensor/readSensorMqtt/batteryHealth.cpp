#include "userDef.h"
#include "batteryHealth.h"
#include "KHomeMqttClient.h"
#include <Arduino.h>

#define ADC_REF_VOLT 3.3

void sendNotificationOfBatteryStatus()
{   
  float aVal = 0.0;
  char buf[8]=" ";
  aVal = analogRead(A0);  
  aVal = aVal* (ADC_REF_VOLT/1024);
  
  snprintf(buf,4,"%f",aVal);
  Serial.println(buf);
  
  publishTopic(pubTopic1, buf);
  
}