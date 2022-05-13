#include "slaveDeviceInfo.h"

static SLAVE_DEVICE_INFO slaveDevInfo;

WS_SINT32 getSlaveDeviceInfo( SLAVE_DEVICE_INFO *dInfo)
{
  ws_memcpy((void*)dInfo, (void*)&slaveDevInfo, sizeof(SLAVE_DEVICE_INFO));
  return WS_SUCCESS;
}

WS_SINT32 setSlaveDeviceWiFiInfo( WS_UINT8*mac, WS_UINT8 WiFichannelId, WS_UINT8 WiFiType)
{
  slaveDevInfo.deviceMac[0] = mac[0];
  slaveDevInfo.deviceMac[1] = mac[1];
  slaveDevInfo.deviceMac[2] = mac[2];
  slaveDevInfo.deviceMac[3] = mac[3];
  slaveDevInfo.deviceMac[4] = mac[4];
  slaveDevInfo.deviceMac[5] = mac[5];  
  slaveDevInfo.wifiClass = WiFichannelId;
  slaveDevInfo.wifiType = WiFiType;
  return WS_SUCCESS; 
}

WS_SINT32 setSlaveDeviceName(WS_UINT8 *devName)
{
  ws_memcpy(slaveDevInfo.deviceName, devName, MAX_NAME_SIZE);
  return WS_SUCCESS;
}
WS_SINT32 setSlaveDeviceType(WS_UINT8 devType)
{
  slaveDevInfo.deviceType= devType;
  return WS_SUCCESS;
}