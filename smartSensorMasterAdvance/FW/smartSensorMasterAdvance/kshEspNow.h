#ifndef SLAVE_ESP_NOW_H_
#define SLAVE_ESP_NOW_H_

#include "userDef.h"

WS_SINT32 initKHomeEspNow();
WS_SINT32 slaveSendData(WS_UINT8* dstMac, WS_UINT8 *dataBuf, WS_UINT16 msgLen );

#endif //SLAVE_ESP_NOW_H_