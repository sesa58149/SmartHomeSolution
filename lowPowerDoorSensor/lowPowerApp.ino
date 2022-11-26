
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<Esp.h>
#include<Arduino.h>
#include "RTCMemLib.h"
#include "IOManager.h"
#include "eepromConf.h"

const char* ssid = "subhSpec";
const char* password = "pawan@158";

#define IN0 D1
#define IN1 D2
#define MAX_SENSOR_SUPPORTED  2
#define ADC_BIT   10
#define VCC_REF   3.2

// initialize static variable objectCnt before initiating the object
int inputOutput::objectCnt=0;

inputOutput inSensor1(IN0, PIN_DIR_IN);
inputOutput inSensor2(IN1, PIN_DIR_IN);
inputOutput led(D6, PIN_DIR_OUT);
inputOutput configButton(D5, PIN_DIR_IN);
inputOutput batteryLevel(A0, PIN_DIR_IN);

eepromConf eepConf;
deviceConf devConfig;

typedef enum
{
 DOOR_STATE_UNDEF=0,
 DOOR_STATE_OPEN,
 DOOR_STATE_CLOSE 
}DOOR_STATE;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(eepConf.ssid, eepConf.pass);

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

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  devConfig.getDevConf(&eepConf);

  Serial.print("EEPROM valid : ");
  Serial.println(eepConf.valid);

  Serial.print("EEPROM DeviceName : ");
  Serial.println(eepConf.name);
  
  Serial.print("EEPROM SSID : ");
  Serial.println(eepConf.ssid);
    
  Serial.print("EEPROM PASS : ");
  Serial.println(eepConf.pass);
  
  inSensor1.loadLastVal();
  Serial.print("Sensor1 loaded data = ");
  Serial.println(inSensor1.getLastVal());
  
  inSensor2.loadLastVal();
  Serial.print("Sensor2 loaded data = ");
  Serial.println(inSensor2.getLastVal());

  analogProperties adcP;
  adcP.adcBits =ADC_BIT;
  adcP.vccRef = VCC_REF;
  batteryLevel.setAdcProperty(&adcP);

}

float factor = (3.2/1024.0);
void readADC()
{
  float val = analogRead(A0);
  Serial.print("ADC readring    = ");
  Serial.println(val);
  float v = val*factor;
  Serial.print("ADC read voltage   =  ");
  Serial.println(v); 
}

void checkForConfigMode()
{
  int  val = configButton.pinRead();
  if(val == 0)
  {
    delay(500);
    val = configButton.pinRead();
    if(val==0)
    {
      Serial.println("Device entre into the configuration mode");
      while(1)
      {
        led.pinWrite(0);
        delay(500);
        led.pinWrite(1);
        delay(500);
      }
    }
  }  
}

void loop() {
  
  bool isSensor1Changed = inSensor1.isInputChanged();
  bool isSensor2Changed = inSensor2.isInputChanged();
  digitalWrite(LED_BUILTIN, true);  // off
  checkForConfigMode();
  digitalWrite(LED_BUILTIN, false); //on
  
 if(isSensor1Changed == true || isSensor2Changed == true)
 {
    setup_wifi();
    delay(10000);
    WiFi.disconnect();
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


  Serial.println("batteryLevel  = ");
  Serial.println(batteryLevel.getAdcPinVal());
  

  //Turn off the LED before going into the deep sleep.
  digitalWrite(LED_BUILTIN, true); 
  ESP.deepSleep(0x989680); // deep sleep for 10 Sec

}

