#ifndef _SLAVE_DEV_INFO_H
#define _SLAVE_DEV_INFO_H

#include "userDef.h"

#define MAX_NAME_SIZE 16
typedef struct
{
  WS_UINT8 deviceName[16];
  WS_UINT8 deviceMac[6];
  WS_UINT8 deviceType;
  WS_UINT8 wifiClass;
  WS_UINT8 wifiType;
}SLAVE_DEVICE_INFO;

WS_SINT32 getSlaveDeviceInfo( SLAVE_DEVICE_INFO *dInfo);
WS_SINT32 setSlaveDeviceName(WS_UINT8 *devName);
WS_SINT32 setSlaveDeviceWiFiInfo(  WS_UINT8 *mac, WS_UINT8 WiFichannelId,WS_UINT8 WiFiType);
WS_SINT32 setSlaveDeviceType(WS_UINT8 devType);


#endif