#include "ESP8266WiFi.h"  
#include "KHomeTask.h"
#include "userTaskDef.h"
#include "KHomeMqttClient.h"
#include "batteryHealth.h"

#define ESP_NOW_CH_ID 11

void print_wakeup_reason();

ws_TCB wsTaskDec[]={
  //taskId,     taskHandler,    tick,         lifetime, isRunnable, isStarted
    {0,     wsHousekeeperTask, 10/TIME_TICK,   0,            0,         0},
    {1,     idleTask,          5000/TIME_TICK, 0,            0,         0},    
    {2,     wsStatusCheckTask, 20000/TIME_TICK, 0,            0,         0},
};
#define MAX_TASK  (sizeof(wsTaskDec)/sizeof(ws_TCB))

static WS_UINT8 isDataArived = false;

void printMac(uint8_t *mac)
{
  Serial.print("Src Mac add->");
  Serial.print(mac[0]);
  Serial.print(":");
  Serial.print(mac[1]);
  Serial.print(":");
  Serial.print(mac[2]);
  Serial.print(":");
  Serial.print(mac[3]);
  Serial.print(":");
  Serial.print(mac[4]);
  Serial.print(":");
  Serial.print(mac[5]);
  Serial.println(" ");
}

void connectWiFI()
{
  int scanRtry=0;
  WiFi.mode(WIFI_AP_STA); 
  WiFi.begin("subhSpec", "pawan@158");
  
  while (WiFi.status() != WL_CONNECTED) {  
    delay(500);   
    Serial.print(".");
    delay(500);
    if( scanRtry++ >=10)
    {
        WiFi.disconnect();
        ESP.restart();
    }        
  }
    
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());
  Serial.print ("MAC Address :");
  Serial.println(WiFi.macAddress());
}


void setup() {  
  Serial.begin(115200);

  connectWiFI();  
  
  registerTasks(wsTaskDec,MAX_TASK);       
  
  pubSubInit();  
}

void loop() {
  
  startTask();  
  sendNotificationOfBatteryStatus();
  while (1)
  {    
    taskScheduler();
    taskRunner();
    delay(TIME_TICK);    
    
 }//while

}
