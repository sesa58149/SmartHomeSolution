#ifndef _SLAVE_ESP_NOW_PDU
#define _SLAVE_ESP_NOW_PDU

#include "userDef.h"
#include "slaveConfMgr.h"

typedef enum
{
  OPCODE_READ_DIO,                  //0
  OPCODE_READ_DIO_RES,              //1
  OPCODE_WRITE_DIO,                 //2
  OPCODE_WRITE_DIO_RES,             //3
  OPCODE_READ_AIO,                  //4
  OPCODE_READ_AIO_RES,              //5
  OPCODE_WRITE_AIO,                 //6
  OPCODE_WRITE_AIO_RES,
  OPCODE_READ_DEV_INFO,
  OPCODE_READ_DEV_INFO_RES,
  OPCODE_READ_DEV_CONF,
  OPCODE_READ_DEV_CONF_RES,
  OPCODE_WRITE_DEV_CONF,
  OPCODE_WRITE_DEV_CONF_RES,
  OPCODE_WHOIS,
  OPCODE_IAM,
  OPCODE_EVENT,
  OPCODE_EVENT_RES,
  OPCODE_PASS_THROUGH,
  OPCODE_PASS_THROUGH_RES
}ESP_OPCODE;

typedef struct
{
  WS_UINT8 opcode;
  WS_UINT8 dataLen;
}pduHeader;

WS_SINT32 parseSlaveEspNowPdu( WS_UINT8 *macAdd, WS_UINT8* buff);

WS_SINT32 slaveSendWhoIs();
WS_SINT32 slaveWriteDevConf(SLAVE_CONF* conf);

/*
OPCODE,LEN, DATA
read DIO 
Request [opcode|dataLen|(gpioNumber or All(0xFF) )]
Response[opcode|dataLen|ERRCODE|(gpioNumber or All(0xFF) ) | data[2]  all in bit value]
write DIO
Request [opcode|dataLen|(gpioNumber or all 0xFF)| data = set/reset  
Response[opcode|dataLen|ERRCode|  
read device info
Request [opcode|dataLen|data]
response[opcode|dataLen|ERRCODE|deviceName[16]|deviceMac[6]|deviceType]
whoIs
Request [opcode|dataLen|data]
Iam
Response [opcode|dataLen|deviceName[16]|deviceMac[6]|deviceType|wifiClass|wifitype]
read conf
Request [opcode|dataLen|data]
Response[opcode|dataLen|ERRCODE|deviceName[16]|masterMac[6]|deviceType|wifiClass|wifitype]
write conf
Request [opcode|dataLen|deviceName[16]|masterMac[6]|deviceType|wifiClass|wifitype]
Response[opcode|dataLen|ERRCODE|]
event
Request [opcode|dataLen|data]
Response[opcode|dataLen|ERRCODE]

read/write AIO, 4*n
*/


#endif //_SLAVE_ESP_NOW_PDU