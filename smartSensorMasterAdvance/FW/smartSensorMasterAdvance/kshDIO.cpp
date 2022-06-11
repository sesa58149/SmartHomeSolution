#include "kshDIO.h"
WS_UINT16 slaveGpio=0x0000;

WS_SINT32 readDio( WS_UINT8 nIO, WS_UINT8 *input)
{
  WS_UINT16 stats = 0;
  WS_UINT16 *in = (WS_UINT16 *) input;
  WS_SINT32 retVal = WS_SUCCESS;
  if( input == NULL)
    return WS_ERROR;
  if ( ( nIO > 0 ) && (nIO <= 16))
  {
    stats = slaveGpio & (1<< (nIO-1) );
  }
  else if( nIO == 0xFF)  
  {
    stats = slaveGpio;
  }
  else
  {
    retVal = WS_ERROR;
  }
  *in = stats;
  return retVal;
}

// byte gpionumer + byte set/reset
WS_SINT32 writeDio( WS_UINT8 dataLen, WS_UINT8* pduData)
{
  WS_SINT32 retVal = WS_SUCCESS;
  WS_UINT8 nGpio =0 , stas=0;
  nGpio = pduData[0];
  stas = pduData[1];
  if( (dataLen == 2) && ( stas <= 1) )
  {
    if(nGpio == 0xFF)
    {
      slaveGpio = stas;
    }
    else
    {
      if(stas == 0)
      {
        slaveGpio = slaveGpio & ~(1 << (nGpio -1));
      }
      else
      {
        slaveGpio = slaveGpio & (1 << (nGpio -1));
      }
    }
  }
  else
  {
   retVal= WS_ERROR;
  }
  return retVal;
}

// byte gpionumer 
WS_SINT32 toggleDio( WS_UINT8 dataLen, WS_UINT8* pduData)
{
  WS_SINT32 retVal = WS_SUCCESS;
  WS_UINT8 nGpio =0 , stas=0;
  nGpio = pduData[0];
  
  if( dataLen == 1) 
  {
    if(nGpio == 0xFF)
    {
      slaveGpio ^= 0xFF;
    }
    else
    {
      slaveGpio ^= (1 << (nGpio -1));
    }
  }
  else
  {
   retVal= WS_ERROR;
  }
  return retVal;
}
