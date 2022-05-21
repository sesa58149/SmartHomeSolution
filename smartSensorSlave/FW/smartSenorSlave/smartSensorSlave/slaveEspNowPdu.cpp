#include "arduino.h"
#include "slaveEspNowPdu.h"
#include "slaveEspNow.h"
#include "slaveDeviceInfo.h"
#include "slaveDIO.h"
#include "slaveConfMgr.h"

extern configManager confMgr;
WS_UINT8 responseData[128] = { 0 };
WS_UINT8 tmpBuf[128] = { 0 };
extern WS_UINT8 bMac[6];
GPIO_RESPONSE_BUFFER gpioResBuff = { 0,0,0,0};
COM_STS comStatus = COM_STS_AVAILABLE;
extern void printDevInfo(SLAVE_CONF * info);

void releaeGipoResponse()
{
  gpioResBuff.isValid = false;
}
GPIO_RESPONSE_BUFFER * getGpioResponse()
{
  return & gpioResBuff;
}
COM_STS getComSts()
{
  return comStatus;
}
void setComSts(COM_STS sts)
{
  comStatus = sts;
}
WS_SINT32 encodeReadDIOResponse(WS_SINT32 errCode, WS_UINT8 pinNm ,WS_UINT8* dstBuf, WS_UINT8* srcBuf)
{
  
  dstBuf[0]= OPCODE_READ_DIO_RES;
  dstBuf[2] = (WS_UINT8)errCode;
  if(errCode != 0)
  {
    dstBuf[1] = 1; // len 1 = BYTE ERR_CODE
  }
  else
  {
    dstBuf[1] = 3; // len =1 BYTE ERR_CODE 2 BYTE GPIO STATUS
    dstBuf[3] = pinNm;
    dstBuf[4] = srcBuf[0];
    dstBuf[5] = srcBuf[1];
  } 
  return (dstBuf[1]+sizeof(pduHeader) ); // total number of bytes to be sent
}
//Request [opcode|dataLen|(gpioNumber or All(0xFF) )]
WS_SINT32 encodeReadDIOReq( WS_UINT8* dstBuf, WS_UINT8 pinId)
{  
  dstBuf[0] = OPCODE_READ_DIO;
  dstBuf[1] = 1; // len = always 1 byte
  dstBuf[2] = pinId; 

  return dstBuf[1];
}
//Request [opcode|dataLen|(gpioNumber or all 0xFF)| data = 2 byte   
WS_SINT32 encodeWriteDIOReq(WS_UINT8* dstBuf, WS_UINT8 pinId, WS_UINT16 gpioVal)
{
  dstBuf[0] = OPCODE_WRITE_DIO;
  dstBuf[1] = 3;
  dstBuf[2] = pinId;
  dstBuf[3] = (WS_UINT8)(gpioVal & 0x00ff);
  dstBuf[4] = (WS_UINT8) ((gpioVal & 0xff00) >> 8);
  return dstBuf[1];
}
void decodeReadDIOResponse(WS_UINT8* dstBuf, WS_UINT8 len)
{
  gpioResBuff.isValid = 0;
  if( (dstBuf[0] == WS_SUCCESS) &&  (len == 3) )
  {
    gpioResBuff.isValid = true;
    gpioResBuff.pinId = dstBuf[3];
    gpioResBuff.pinVal[0] = dstBuf[4];
    gpioResBuff.pinVal[1] = dstBuf[5]; 
  }
}

WS_SINT32 encodeWriteDIOResponse(WS_SINT32 errCode, WS_UINT8* dstBuf )
{
  
  dstBuf[0]= OPCODE_WRITE_DIO_RES;
  dstBuf[2] = (WS_UINT8)errCode;
  dstBuf[1] = 1; // 1 BYTE ERR_CODE
 
  return (dstBuf[1]+sizeof(pduHeader) ); // total number of bytes to be sent
}

WS_SINT32 encodeWriteDev_Conf_res(WS_SINT32 errCode, WS_UINT8* dstBuf) 
{
  dstBuf[0]= OPCODE_WRITE_DEV_CONF_RES;
  dstBuf[2] = (WS_UINT8)errCode;
  dstBuf[1] = 1; // 1 BYTE ERR_CODE
 
  return (dstBuf[1]+sizeof(pduHeader) ); // total number of bytes to be sent
}

WS_SINT32 encodeIamMsg(WS_UINT8* dstBuf, WS_UINT8* srcBuf)
{
  dstBuf[0] = OPCODE_IAM;
  dstBuf[1] = sizeof(SLAVE_DEVICE_INFO);
  ws_memcpy((void*)&dstBuf[2],srcBuf, sizeof(SLAVE_DEVICE_INFO));
  return (dstBuf[1]+sizeof(pduHeader) ); // total number of bytes to be sent
}

void printIamMsg(SLAVE_DEVICE_INFO *info)
{
  char buf[32] = " ";

  sprintf(buf, "%s", info->deviceName);
  Serial.println("");
  Serial.println(buf);

  Serial.print("Master MAC :");
  Serial.print(info->deviceMac[0], HEX);
  Serial.print(" : ");
  Serial.print(info->deviceMac[1], HEX);
  Serial.print(" : ");
  Serial.print(info->deviceMac[2], HEX);
  Serial.print(" : ");
  Serial.print(info->deviceMac[3], HEX);
  Serial.print(" : ");
  Serial.print(info->deviceMac[4], HEX);
  Serial.print(" : ");
  Serial.println(info->deviceMac[5], HEX);

  Serial.println(info->deviceType, HEX);
  Serial.println(info->wifiType, DEC);
  Serial.println(info->wifiClass,DEC);
}
WS_SINT32 decodeIAmMsg(WS_UINT8* pduData)
{
  SLAVE_DEVICE_INFO *devInfo = (SLAVE_DEVICE_INFO*) pduData;
  printIamMsg( devInfo);   
  return WS_SUCCESS;
}

WS_SINT32 encodeReadConf( WS_UINT8* responseData, SLAVE_CONF *sConf)
{
  responseData[0] = OPCODE_READ_DEV_CONF_RES;
  responseData[1] = sizeof(SLAVE_CONF);
  ws_memcpy((void*)&responseData[2],(WS_UINT8*)sConf, sizeof(SLAVE_CONF));
  return (responseData[1]+sizeof(pduHeader) ); // total number of bytes to be sent 
}

void sendIamMsg(WS_UINT8 *mAdd)
{
  WS_SINT16 resPDULen =0 ;
  getSlaveDeviceInfo( (SLAVE_DEVICE_INFO *)tmpBuf);
  resPDULen = encodeIamMsg(responseData ,tmpBuf);
  slaveSendData(mAdd,responseData, resPDULen);
}

WS_SINT32 parseSlaveEspNowPdu( WS_UINT8 *macAdd, WS_UINT8* buff)
{
  WS_SINT32 retVal=WS_SUCCESS;
  WS_SINT16 resPDULen =0 ;
  pduHeader *pHeader = (pduHeader*)buff;
  WS_UINT8 * pduData = buff+ sizeof(pduHeader);
  SLAVE_CONF* sConf = NULL;

  switch(pHeader->opcode)
  {
    case OPCODE_READ_DIO:
    if( pHeader->dataLen == 1)
    {
      retVal = readDio(pduData[0], tmpBuf);
      resPDULen = encodeReadDIOResponse(retVal, pduData[0], responseData, tmpBuf);
      if (resPDULen != WS_ERROR)
      {
       slaveSendData(macAdd,responseData, resPDULen);
      }
    }
    break;
    case OPCODE_READ_DIO_RES:
    {      
      decodeReadDIOResponse(pduData, pHeader->dataLen);      
      setComSts(COM_STS_AVAILABLE);
    }
    break;
    case OPCODE_WRITE_DIO:
    if(pHeader->dataLen == 2)
    {
      retVal = writeDio(pHeader->dataLen, pduData);
      resPDULen =encodeWriteDIOResponse(retVal,responseData) ;
      if (resPDULen != WS_ERROR)
      {
       slaveSendData(macAdd,responseData, resPDULen);
      }
    }
    
    break;
    case OPCODE_WRITE_DIO_RES:
      setComSts(COM_STS_AVAILABLE);
    break;
    case OPCODE_READ_AIO:
    break;
    case OPCODE_WRITE_AIO:
    break;
    case OPCODE_READ_DEV_INFO:
    break;
    case OPCODE_READ_DEV_CONF:
    Serial.println("Get Config Received ===>>");
    sConf= confMgr.getDeviceConf();
    resPDULen = encodeReadConf( responseData, sConf);
    if (resPDULen != WS_ERROR)
    {
       slaveSendData(macAdd,responseData, resPDULen);
    }
    break;
    case OPCODE_WRITE_DEV_CONF: 
    Serial.println("set Config Received ===>>");
    if(pHeader->dataLen == sizeof(SLAVE_CONF))
    {
      retVal = confMgr.putDeviceConf( (SLAVE_CONF*) pduData );
      printDevInfo( (SLAVE_CONF *) pduData);
      resPDULen =encodeWriteDev_Conf_res(retVal,responseData) ;
      if (resPDULen != WS_ERROR)
      {
       slaveSendData(macAdd,responseData, resPDULen);
      }
    }
    break;
    case OPCODE_WHOIS:
    Serial.println("Who Is Received ===>>");
    sendIamMsg(macAdd);    
    break;
    case OPCODE_IAM:
    Serial.println("I Am Received ===>>");
    if(pHeader->dataLen <= sizeof(SLAVE_DEVICE_INFO))
    {
      decodeIAmMsg(pduData);
    }
    break;
    case OPCODE_EVENT:
    break;
    default:
    retVal = WS_ERROR;
    Serial.println("Wrong Opcode reveived");
    break; 
  }
  return retVal;
}
WS_SINT32 encodeWhoIsMsg(WS_UINT8* inputBuf)
{
  inputBuf[0] = OPCODE_WHOIS;
  inputBuf[1] = 0 ;// no data
  return (inputBuf[1]+sizeof(pduHeader) ); // total number of bytes to be sent
}
WS_SINT32 slaveSendWhoIs()
{
  WS_SINT16 reqPDULen =0 ;
  reqPDULen = encodeWhoIsMsg( tmpBuf);
  slaveSendData(bMac,tmpBuf, reqPDULen);
  return WS_SUCCESS;
}

WS_SINT32 encodeWriteDevConf(WS_UINT8* dstBuf ,WS_UINT8* srcBuf, WS_UINT8  len)
{
  dstBuf[0] = OPCODE_WRITE_DEV_CONF;
  dstBuf[1] = len ;// no data
  ws_memcpy( (void*) &dstBuf[2], (void*)srcBuf, len);
  return (dstBuf[1]+sizeof(pduHeader) ); // total number of bytes to be sent
}

WS_SINT32 slaveWriteDevConf(SLAVE_CONF* conf)
{
  WS_SINT16 reqPDULen =0 ;
  reqPDULen = encodeWriteDevConf( tmpBuf,(WS_UINT8*) conf, sizeof(SLAVE_CONF));
  slaveSendData(bMac,tmpBuf, reqPDULen);
  return WS_SUCCESS;
}

WS_SINT32 slaveGpioRead( WS_UINT8 *mAdd,  WS_UINT8 pinId)
{
  WS_SINT16 reqPDULen =0 ;
  WS_SINT32 retVal = WS_SUCCESS;
  reqPDULen = encodeReadDIOReq(tmpBuf, pinId);
  slaveSendData(mAdd,tmpBuf, reqPDULen);
  setComSts(COM_STS_BUSY);
  return retVal;
}
WS_SINT32 slaveGpioWrite(WS_UINT8 *mAdd, WS_UINT8 pinId  , WS_BOOL  isOn)
{
  WS_SINT16 reqPDULen =0, gpioVal = 0 ;
  WS_SINT32 retVal = WS_SUCCESS;
  if( pinId >= 16)
    return WS_ERROR;
  if(isOn == true)
  {
    gpioVal |= 1 <<  pinId;
  } // dafult is reseted
  
  reqPDULen = encodeWriteDIOReq(tmpBuf, pinId, gpioVal);
  slaveSendData(mAdd,tmpBuf, reqPDULen);
  setComSts(COM_STS_BUSY);

  return retVal;
}