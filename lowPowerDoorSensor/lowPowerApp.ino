
//#include <ESP8266WiFi.h>
//#include <PubSubClient.h>
#include<Esp.h>
#include<Arduino.h>
#include "RTCMemLib.h"
#include "IOManager.h"

const char* ssid = "subhSpec";
const char* password = "pawan@158";

#define IN0 D1
#define IN1 D2
#define MAX_SENSOR_SUPPORTED  2

int inputOutput::objectCnt=0;

inputOutput inSensor1(IN0,PIN_DIR_IN);
inputOutput inSensor2(IN1,PIN_DIR_IN);

typedef enum
{
 DOOR_STATE_UNDEF=0,
 DOOR_STATE_OPEN,
 DOOR_STATE_CLOSE 
}DOOR_STATE;
/*
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
*/
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  
  inSensor1.loadLastVal();
  Serial.print("Sensor1 loaded data = ");
  Serial.println(inSensor1.getLastVal());
  
  inSensor2.loadLastVal();
  Serial.print("Sensor2 loaded data = ");
  Serial.println(inSensor2.getLastVal());

}

// bool isInputChanged( rtcStore *rtcVal, inputOutput *sensor)
// {
//   bool retVal = false;
//   int inBit = sensor->readPin(); 
//   int dState = (int)DOOR_STATE_UNDEF;

//   Serial.print("Read input =  ");
//   Serial.println(inBit);
//   if(inBit == true)
//   {
//     dState = (int)DOOR_STATE_OPEN;
//   }
//   else {
//     dState = (int)DOOR_STATE_CLOSE;
//   }
//   Serial.print("DOOR_STATE dState   =  ");
//   Serial.println(dState);
//   if(rtcVal->input !=  dState)
//   {
//     rtcVal->input = dState;    
//     retVal = true;    
//   }

//   return retVal;
// }




// void loop() {
  
//   readFromRTCMemory(&rtcData[0], MAX_SENSOR_SUPPORTED*sizeof(rtcStore));
//   Serial.print("DOOR_STATE last state1   =  ");
//   Serial.println(rtcData[0].input);
//   Serial.print("DOOR_STATE last state2   =  ");
//   Serial.println(rtcData[1].input);
  
//   bool isSensor1Changed = isInputChanged(&rtcData[0], &inSensor1);
//   bool isSensor2Changed = isInputChanged(&rtcData[1], &inSensor2);
 
//  if(isSensor1Changed == true || isSensor2Changed == true)
//  {
//     writeToRTCMemory(&rtcData[0], MAX_SENSOR_SUPPORTED*sizeof(rtcStore));
//     delay(100);
//     //setup_wifi();
//     delay(10000);
//    // WiFi.disconnect();
//     Serial.println("Wifi disconnecting");
//     Serial.print("Written value to RTC0 **************");
//     Serial.println(rtcData[0].input);
//     Serial.print("Written value to RTC0 **************");
//     Serial.println(rtcData[1].input);
//  }
  
//    Serial.println("going to deepsleep mode");
//     //Turn off the LED before going into the deep sleep.
//   digitalWrite(LED_BUILTIN, true);
//   ESP.deepSleep(0x989680); // deep sleep for 10 Sec

// }


void loop() {
  
  bool isSensor1Changed = inSensor1.isInputChanged();
  bool isSensor2Changed = inSensor2.isInputChanged();
 
 if(isSensor1Changed == true || isSensor2Changed == true)
 {
    // writeToRTCMemory(&rtcData[0], MAX_SENSOR_SUPPORTED*sizeof(rtcStore));
    // delay(100);
    //setup_wifi();
    delay(10000);
   // WiFi.disconnect();
    Serial.println("Wifi disconnecting");
    Serial.print("Written value to RTC0 **************");
    Serial.println(inSensor1.getLastVal());
    Serial.print("Written value to RTC0 **************");
    Serial.println(inSensor2.getLastVal());
   
 }
  
  Serial.print("Number of IO object created     ");
  Serial.println(inputOutput::objectCnt);
  Serial.print("Object ID of sensor 1 == ");
  Serial.println(inSensor1.getObjId());
  Serial.print("Object ID of sensor 2 == ");
  Serial.println(inSensor2.getObjId());
  Serial.println("going to deepsleep mode");
    //Turn off the LED before going into the deep sleep.
  digitalWrite(LED_BUILTIN, true);
  ESP.deepSleep(0x989680); // deep sleep for 10 Sec

}

