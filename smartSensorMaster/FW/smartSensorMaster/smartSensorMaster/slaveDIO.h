#ifndef _SLAVEDIO_H
#define _SLAVEDIO_H
#include "userDef.h"

// read GPIO n
// nIO = gpio number/0xff (complete gpio max=16) 
// *input == return buffer of 2 bytes
WS_SINT32 readDioPin( WS_UINT8 nIO, WS_UINT8 *input);
WS_SINT32 readDioAll(WS_UINT8 *input);
// write gpio
// *pduData = data frame received from network gipo number + command (set/reset)
// 0xff -- set/reset to all gpio
WS_SINT32 writeDioPin( WS_UINT8 dataLen, WS_UINT8* pduData);
WS_SINT32 writeDioAll( WS_UINT8 dataLen, WS_UINT8* pduData);

WS_SINT32 toggleDio( WS_UINT8 dataLen, WS_UINT8* pduData);

#endif // _SLAVEDIO_H