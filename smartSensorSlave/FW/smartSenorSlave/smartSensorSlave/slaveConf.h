#ifndef _SLAVE_CONF_H_
#define _SLAVE_CONF_H_

#include "userDef.h"

#define DEFAULT_DEVICE_NAME "KSHOME"
#define DEFAULT_DEVICE_TYPE KS_DEVICE_TYPE_SALAVE
#define VALID_STR "KSH"

typedef enum{
KS_DEVICE_TYPE_SALAVE=0,
KS_DEVICE_TYPE_MASTER,
KS_DEVICE_TYPE_MASTER_ADVANCE
}KS_DEVICE_TYPE;

typedef enum
{
  SLAVE_AP_STA,
  SLAVE_STA
}SLAVE_WIFI_TYPE;

typedef struct
{
  WS_UINT8 validStr[4];
  WS_UINT8 masterMac[6];
  WS_UINT8 deviceName[16];
  WS_UINT8 deviceType;
  WS_UINT8 wifiClass;
  WS_UINT8 wifiType;
}SLAVE_CONF;


#endif // _SLAVE_CONF_H_