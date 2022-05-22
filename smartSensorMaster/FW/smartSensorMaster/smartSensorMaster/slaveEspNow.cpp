#include "arduino.h"
#include <espnow.h>
#include "slaveEspNow.h"
#include "slaveEspNowPdu.h"
extern WS_UINT8 bMac[6];
void onDataRecvCmd(  WS_UINT8 *macAdd,  WS_UINT8 *data, int len);
void onDataSentCmd( WS_UINT8 *macAdd, WS_UINT8 sts);

static WS_UINT8 isDataArived = false;

void onDataRecvCmd(  WS_UINT8 *macAdd,  WS_UINT8 *data, int len)
{
  WS_UINT8 tmpBuf[32] = { 0 };
  char tmpBuf1[32] = { 0 };

  /*isDataArived = true; 
  if( len > 0xFF)
  {
    Serial.println("corrupt packet received");
    return;
  }
  ws_memcpy(tmpBuf, data, len);
  sprintf(tmpBuf1, "%s", tmpBuf);
  Serial.println(tmpBuf1);
  */
  parseSlaveEspNowPdu(macAdd, data);
}

void onDataSentCmd( WS_UINT8 *macAdd, WS_UINT8 sts)
{
  if (sts == WS_SUCCESS)
    Serial.println("data send success");
  else
    Serial.println("data send fail");
}
WS_SINT32 initKHomeEspNow()
{
  WS_SINT32 retVal = WS_ERROR;
  
  if(esp_now_init()==0)
  {
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_recv_cb((esp_now_recv_cb_t) onDataRecvCmd);
    esp_now_register_send_cb((esp_now_send_cb_t)onDataSentCmd);

    esp_now_add_peer(bMac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    
    retVal = WS_SUCCESS;
  }
  return retVal; 
  
}
WS_SINT32 slaveSendData(WS_UINT8* dstMac, WS_UINT8 *dataBuf, WS_UINT16 msgLen )
{
  if( dstMac == NULL || dataBuf == NULL || msgLen > 0xFF)
    return WS_ERROR;
  esp_now_send(dstMac, dataBuf, msgLen);
  //Serial.println("data sent");
  return WS_SUCCESS;  
}