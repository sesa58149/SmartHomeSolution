#include "ESP8266WiFi.h"
#include "slaveConfMgr.h"
#include "slaveDeviceInfo.h"
#include "slaveEspNow.h"
#include "slaveEspNowPdu.h"
configManager confMgr;

#define FW_VER  "0.0.002"

void printDevInfo(SLAVE_CONF * info);

void printDevInfo(SLAVE_CONF * info)
{
  char buf[32] = " ";

  sprintf(buf, "%s", info->deviceName);
  Serial.println("");
  Serial.println(buf);

  Serial.print("Master MAC :");
  Serial.print(info->masterMac[0], HEX);
  Serial.print(" : ");
  Serial.print(info->masterMac[1], HEX);
  Serial.print(" : ");
  Serial.print(info->masterMac[2], HEX);
  Serial.print(" : ");
  Serial.print(info->masterMac[3], HEX);
  Serial.print(" : ");
  Serial.print(info->masterMac[4], HEX);
  Serial.print(" : ");
  Serial.println(info->masterMac[5], HEX);

  Serial.println(info->deviceType, HEX);
  Serial.println(info->wifiType, DEC);
  Serial.println(info->wifiClass,DEC);

}

WS_SINT32 initWiFi()
{
  WS_UINT8 mac[6];
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.print("Station MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  setSlaveDeviceWiFiInfo( mac, WiFi.channel(), WIFI_STA);
  return 0;
}

void printDInfo(SLAVE_DEVICE_INFO *devInfo)
{
  char buf[32] = " ";

  Serial.print("devceName -> ");
  sprintf(buf, "%s", devInfo->deviceName);
  Serial.println(buf);

  Serial.println(" ");
  Serial.print("MAC -> ");
  Serial.print(devInfo->deviceMac[0], HEX);
  Serial.print(" : ");
  Serial.print(devInfo->deviceMac[1], HEX);
  Serial.print(" : ");
  Serial.print(devInfo->deviceMac[2], HEX);
  Serial.print(" : ");
  Serial.print(devInfo->deviceMac[3], HEX);
  Serial.print(" : ");
  Serial.print(devInfo->deviceMac[4], HEX);
  Serial.print(" : ");
  Serial.print(devInfo->deviceMac[5], HEX);
  Serial.println(" ");
  Serial.print( "deviceType -> ");
  Serial.println(devInfo->deviceType);
  Serial.print( "wifiClass -> ");
  Serial.println(devInfo->wifiClass);
  Serial.print( "wifiType -> ");
  Serial.println(devInfo->wifiType);
}

SLAVE_DEVICE_INFO dInfo;
WS_UINT8 bMac[6] = {0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF};
void setup() {
  // put your setup code here, to run once:
  SLAVE_CONF *devConf = 0;
  Serial.begin(115200);
  configManager confMgr;
  devConf = confMgr.getDeviceConf();  
  printDevInfo(devConf);
  setSlaveDeviceName(devConf->deviceName);
  setSlaveDeviceType(devConf->deviceType);
  // initialize wifi
  initWiFi();
  getSlaveDeviceInfo( &dInfo);
  printDInfo(&dInfo);
  //initialize ESP-NOW
  initKHomeEspNow();

  delay(1000);
  sendIamMsg(bMac);

  delay(1000);
   
}

void writeDevConf()
{
  SLAVE_CONF conf;
  conf.validStr[0] = 'K';
  conf.validStr[1] = 'S';
  conf.validStr[2] = 'H';

  conf.masterMac[0] = 0X62;
  conf.masterMac[1] = 0X39;
  conf.masterMac[2] = 0X46;
  conf.masterMac[3] = 0X51;
  conf.masterMac[4] = 0X91;
  conf.masterMac[5] = 0X40;
  ws_memcpy(conf.deviceName, "KSHOME_MSTR", sizeof("KSHOME_MSTR"));
  conf.deviceType = 0x1;
  conf.wifiClass = 11;
  conf.wifiType = SLAVE_STA;
  slaveWriteDevConf(&conf);
}

WS_SINT32 readGPIO( WS_UINT8 pinId )
{
  WS_SINT32 retVal = WS_ERROR;
  if (getComSts() == COM_STS_AVAILABLE)
  {
    slaveGpioRead(bMac,pinId);
    delay(1000);
    //Serial.println("***** Read command sent******************");
    for (int i=0; i<10; i++);
    {
      if (getComSts() == COM_STS_AVAILABLE) // not busy
      {
        retVal = WS_SUCCESS;
       Serial.println("***** Read GIPO Success******************");
        return retVal;
      }        
        delay(200);
    }
    
  }  
  setComSts(COM_STS_AVAILABLE); // free the channel in case repsonse not recive from the server to avoid infinite waiting 
  return retVal;
}
WS_SINT32 writeGPIO( WS_UINT8 pinId, WS_BOOL pinVal)
{
  WS_SINT32 retVal = WS_ERROR; 
  if (getComSts() == COM_STS_AVAILABLE)
  {
    slaveGpioWrite(bMac, pinId , pinVal);
    delay(500);
    //Serial.println("***** Write command sent******************");
    for (int i=0; i<10; i++);
    {
      if (getComSts() == COM_STS_AVAILABLE) // not busy
      {
        retVal = WS_SUCCESS;
        Serial.println("***** write GIPO success******************");
        return retVal;
      }        
        delay(200);
    }    
  }  
  setComSts(COM_STS_AVAILABLE); // free the channel in case repsonse not recive from the server to avoid infinite waiting 
  return retVal;
}
#define GPIO_LOAD_CON_PIN_NUM  0x04
#define MAX_RETRY 5
void loop() {
  // put your main code here, to run repeatedly:
  WS_BOOL isOn = true; // On
  WS_SINT32 retVal;
  int reTry = 0;
  Serial.println("***** Slave  Device Started ******************");
  Serial.print("FW version : ");
  Serial.println(FW_VER);
  while(1)
  {    
    do {
      retVal = readGPIO(0xFF);
      delay(100);
      reTry++;
    }while( retVal != WS_SUCCESS && reTry < MAX_RETRY);

    GPIO_RESPONSE_BUFFER *gpioInfo = getGpioResponse();    
    if (gpioInfo->isValid == true)
    {
      //Serial.print(" Read GPIO val : ");
      //Serial.print(gpioInfo->pinVal[0], HEX);
      //Serial.print(" : ");
      //Serial.println(gpioInfo->pinVal[1], HEX);
      if( (gpioInfo->pinVal[0]) & (1 << GPIO_LOAD_CON_PIN_NUM) )
      {
        isOn = false;  // toggle the bit
        Serial.println( " Turn ON >> Turn OFF");
      }
      else
      {
        Serial.println( " Turn OFF >> Turn ON");
      }
      reTry = 0;
      do {
        retVal = writeGPIO(GPIO_LOAD_CON_PIN_NUM, isOn);
        delay(100);
        reTry++;
      }while( retVal != WS_SUCCESS && reTry < MAX_RETRY);      
      
    }    
    releaeGipoResponse();
    delay(500);
    ESP.deepSleep(0);
  }

}
