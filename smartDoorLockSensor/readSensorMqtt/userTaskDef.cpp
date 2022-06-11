#include "userTaskDef.h"
#include "batteryHealth.h"
#include "readSensor.h"
#include <Arduino.h> 
#include "KHomeMqttClient.h"

void wsStatusCheckTask(void)
{
 /* static WS_UINT8 ledStatus = 0;
  digitalWrite(STS_LED,ledStatus);  
  ledStatus = ~ledStatus;  */

  Serial.println("wsLEDTask called");

}
void wsHousekeeperTask(void)
{
  //Serial.println("wsHousekeeperTask called");
  pubSubTaskloop();
}

void idleTask(void)
{
   static WS_UINT8 sensorData = 0;
   Serial.println("ESP Now Master sensor Val:  ");  
   sendNotificationOfBatteryStatus();
   sendNotificationReadSensorStatus();
}
void relayHandlerTask(void)
{
  Serial.println("relayHandlerTask called");
}
