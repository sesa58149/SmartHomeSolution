#ifndef _USE_DEF_H__
#define _USE_DEF_H__

#include <string.h>
#include <stdio.h>
#include <arduino.h>

typedef unsigned char WS_UINT8;
typedef signed char WS_SINT8;
typedef unsigned short WS_UINT16;
typedef signed short WS_SINT16;
typedef unsigned long WS_UINT32;
typedef signed long WS_SINT32;
typedef unsigned char WS_BOOL;

#define WS_ERROR -1
#define WS_SUCCESS  0

#define ws_memset(x,y,z) memset(x,y,z)
#define ws_memcpy(x,y,z) memcpy(x,y,z)

#define WAIT_MS(x)  delay(x)

  
void myprint(char* msg );
void myprintln(char* msg );
void tcpdelay(int ms);
void myprintlnData( int err);



#endif //_USE_DEF_H__
