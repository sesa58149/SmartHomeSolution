
#include <ESP8266WiFi.h>
#include "mqttServices.h"
#include<Esp.h>
#include<Arduino.h>
#include "RTCMemLib.h"
#include "IOManager.h"
#include "eepromConf.h"
#include <Arduino_JSON.h>



const char* ssid = "subhSpec";
const char* password = "pawan@158";

#define IN0 D1
#define IN1 D2
#define MAX_SENSOR_SUPPORTED  2
#define ADC_BIT   10
#define VCC_REF   3.2

// initialize static variable objectCnt before initiating the object
int inputOutput::objectCnt=0;

inputOutput inSensor1("sensor1",IN0, PIN_DIR_IN);
inputOutput inSensor2("sensor2",IN1, PIN_DIR_IN);
inputOutput led("statusLEd",D6, PIN_DIR_OUT);
inputOutput configButton("inConfig", D5, PIN_DIR_IN);
inputOutput batteryLevel("batteryLevel", A0, PIN_DIR_IN);

eepromConf eepConf;
deviceConf devConfig;

typedef enum
{
 DOOR_STATE_UNDEF=0,
 DOOR_STATE_OPEN,
 DOOR_STATE_CLOSE 
}DOOR_STATE;


void invalidConfigLedPatter()
{
   for(int i = 0; i< 25; i++)// for 5 sec
   {
    led.pinWrite(1);
    delay(100);
    led.pinWrite(0);
    delay(100);
   }
}

void recvMessage(char* topic, byte* payload, unsigned int length)
{
  bool isValidconfig = true;

  eepromConf tmpConf;
  Serial.println(topic);
  int len = (length>256)?256:length;

  char msg[256]=" ";

  Serial.print( "buf len");
  Serial.println(len);
  for(int i=0; i<(int)len; i++)
  {
    msg[i]=(char)payload[i];
   // Serial.print(msg[i]);
  }
  msg[len]=0x00;
  Serial.println("");
    
  JSONVar myObject = JSON.parse(msg);

  // JSON.typeof(jsonVar) can be used to get the type of the variable
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    invalidConfigLedPatter();
    return;
  }

  Serial.print("JSON.typeof(myObject) = ");
  Serial.println(JSON.typeof(myObject)); // prints: object

  // myObject.hasOwnProperty(key) checks if the object contains an entry for key
  
  int strLen = 0;
  if (myObject.hasOwnProperty("name")) {
    //Serial.print("myObject[\"name\"] = ");
    strLen = strlen( myObject["name"]);
    if(strLen <= MAX_STR_LEN)
    {
      memcpy((void*)&tmpConf.name[0],(const char*)myObject["name"], strLen);
      tmpConf.name[strlen( myObject["name"])] =0x00; // null charactor
      Serial.print("Struct data: ");
      Serial.println(tmpConf.name);
      //Serial.println((const char*) myObject["name"]);
    }
    else{
      isValidconfig = false;
    }
  }
    if (myObject.hasOwnProperty("SSID")) {
    //Serial.print("myObject[\"SSID\"] = ");
    strLen = strlen( myObject["SSID"]);
    if(strLen <= MAX_STR_LEN)
    {
      memcpy((void*)&tmpConf.ssid[0],(const char*)myObject["SSID"], strLen);
      tmpConf.ssid[strlen( myObject["SSID"])] =0x00; // null charactor
      Serial.print("Struct data: ");    
      Serial.println(tmpConf.ssid);
      // Serial.println( strlen( myObject["SSID"]));
      // Serial.println(strlen(tmpConf.ssid));
      //Serial.println((const char*) myObject["SSID"]);
    }
    else{
      isValidconfig = false;
    }
  }
    if (myObject.hasOwnProperty("pass")) {
    //Serial.print("myObject[\"pass\"] = ");
    strLen = strlen(myObject["pass"]);
    if(strLen <= MAX_STR_LEN)
    {
      memcpy((void*)&tmpConf.pass[0],(const char*)myObject["pass"], strLen);
      tmpConf.pass[strlen( myObject["pass"])] =0x00; // null charactor
      Serial.print("Struct data: ");    
      Serial.println(tmpConf.pass);
      //Serial.println((const char*) myObject["pass"]);
    }
    else{
      isValidconfig = false;
    }
  }
    if (myObject.hasOwnProperty("brokerAdd")) {
    //Serial.print("myObject[\"brokerAdd\"] = ");
    strLen = strlen( myObject["brokerAdd"]);
    if(strLen <= MAX_STR_LEN)
    {
      memcpy((void*)&tmpConf.mqttBrokerinfo.mqttBrokerAdd[0],(const char*)myObject["brokerAdd"], strLen);
      tmpConf.mqttBrokerinfo.mqttBrokerAdd[strlen( myObject["brokerAdd"])] =0x00; // null charactor
      Serial.print("Struct data: ");
      Serial.println(tmpConf.mqttBrokerinfo.mqttBrokerAdd);
      //Serial.println((const char*) myObject["brokerAdd"]);
    }
    else{
      isValidconfig = false;
    }
  }
    if (myObject.hasOwnProperty("brokerPort")) {
    //Serial.print("myObject[\"brokerPort\"] = ");
    tmpConf.mqttBrokerinfo.mqttBrokerPort = (int)myObject["brokerPort"];
    Serial.print("Struct data: ");
    Serial.println(tmpConf.mqttBrokerinfo.mqttBrokerPort);
    //Serial.println((int) myObject["brokerPort"]);
  }

  // JSONVars can be printed using print or println
  // Serial.print("myObject = ");
  // Serial.println(myObject);

  // Serial.println();

  //store at eeprom
  if (isValidconfig)
  {
    tmpConf.valid = 0xA5;
    devConfig.setDevConf(&tmpConf);
  }
  else{
    invalidConfigLedPatter();
    Serial.println("invalid configuration");
  }
  

}


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(eepConf.ssid);
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
  
  // Serial.print("EEPROM SSID : ");
  // Serial.println(eepConf.ssid);
    
  // Serial.print("EEPROM PASS : ");
  // Serial.println(eepConf.pass);
  
  Serial.print("BrokerAdd   :");
  Serial.println(eepConf.mqttBrokerinfo.mqttBrokerAdd);
  Serial.print("BrokerPort   :");
  Serial.println(eepConf.mqttBrokerinfo.mqttBrokerPort);
    
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
      setup_wifi();
      mqttServices mqttClient(&eepConf.mqttBrokerinfo, recvMessage);
      if (mqttClient.startMqttServices(eepConf.name))
      {    
        String subTopic = eepConf.name + String("/") + String("config"); 
        char tmp[32];
        subTopic.toCharArray(tmp, 32);  
        
        while (! mqttClient.isConnected());
        mqttClient.subscribeMessage(tmp, 0);
        while(1)
        {
          mqttClient.recvUnblock();
          led.pinWrite(0);
          delay(500);
          led.pinWrite(1);
          delay(500);
        }
      }
    }
  }  
}

void loop() {
  
  bool isSensor1Changed = inSensor1.isInputChanged();
  bool isSensor2Changed = inSensor2.isInputChanged();
  int in0 = inSensor1.pinRead();
  int in1 = inSensor2.pinRead();

  digitalWrite(LED_BUILTIN, true);  // off
  checkForConfigMode();
  digitalWrite(LED_BUILTIN, false); //on

 

 if(isSensor1Changed == true || isSensor2Changed == true)
 {   
    setup_wifi();

    mqttServices mqttClient(&eepConf.mqttBrokerinfo, recvMessage);
    if (mqttClient.startMqttServices(eepConf.name))
    {      
      if (isSensor1Changed)
      {
        String str = eepConf.name + String("/") + String(inSensor1.getName()) + String("/status");
        Serial.print("Topic : ");
        Serial.println(str);
        char topc[64];
        str.toCharArray(topc, 64);
        if( in0 == 0)
          mqttClient.sendMessage(topc, (char*)"Closed");
        else  
          mqttClient.sendMessage(topc, (char*)"Open");
      }

      if (isSensor2Changed)
      {
        String str = eepConf.name + String("/") + String(inSensor2.getName()) + String("/status");
        Serial.print("Topic : ");
        Serial.println(str);
        char topc[64];
        str.toCharArray(topc, 64);
        if( in1 == 0)
          mqttClient.sendMessage(topc, (char*)"Closed");
        else  
          mqttClient.sendMessage(topc, (char*)"Open");
      }        
    }    
    // Serial.println("Wifi disconnecting");
    // Serial.print("Written value to RTC0 **************");
    // Serial.println(inSensor1.getLastVal());
    // Serial.print("Written value to RTC0 **************");
    // Serial.println(inSensor2.getLastVal());    
  }
  
  // Serial.print("Number of IO object created     ");
  // Serial.println(inputOutput::objectCnt);
  // Serial.print("Object ID of sensor 1 == ");
  // Serial.println(inSensor1.getObjId());
  // Serial.print("Object ID of sensor 2 == ");
  // Serial.println(inSensor2.getObjId());
  Serial.println("going to deepsleep mode");


  Serial.println("batteryLevel  = ");
  Serial.println(batteryLevel.getAdcPinVal());
  
  delay(2000); // wait for some time to get mqtt message deliver to the broker
  //Turn off the LED before going into the deep sleep.
  digitalWrite(LED_BUILTIN, true); 
  ESP.deepSleep(0x989680); // deep sleep for 10 Sec

}

