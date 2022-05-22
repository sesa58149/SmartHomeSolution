#include "arduino.h"
#include "slaveDIO.h"
WS_UINT16 slaveGpio=0x0000;

WS_SINT32 readDioPin( WS_UINT8 nIO, WS_UINT8 *input)
{
   WS_SINT32 retVal = WS_SUCCESS;
   WS_UINT16 stats = 0;
  if ( ( nIO >= 0 ) && (nIO < 16))
  {
    stats = slaveGpio & (1<< nIO );
  } 
  else
  {
      retVal = WS_ERROR;
  }
  input[0] = (WS_UINT8)stats & 0x00FF;
  input[1] = (WS_UINT8)stats & (0xFF00 >>8);
  return retVal;
}
// Max 16 ping 
WS_SINT32 readDioAll(WS_UINT8 *input)
{
  WS_SINT32 retVal = WS_SUCCESS;
 
  input[0] = (WS_UINT8)slaveGpio & 0x00FF;
  input[1] = (WS_UINT8) ( (slaveGpio & 0xFF00) >> 8);
  return retVal;
}

// byte gpionumer + 2byte set/reset
WS_SINT32 writeDioPin( WS_UINT8 dataLen, WS_UINT8* pduData)
{
  WS_SINT32 retVal = WS_SUCCESS;
  WS_UINT8 nGpio =0 ;
  WS_UINT16  stas=0; 

  nGpio = pduData[0];
  stas = pduData[2];
  stas = stas << 8;
  stas |= pduData[1];
  
  //Serial.print(" Data to Write :  ");
  //Serial.println(stas, DEC);
  
  if ( ( nGpio >= 0 ) && (nGpio < 16))
  {
    if(stas & (1<<nGpio) )
    {
      Serial.println("Set output");
      slaveGpio |= (1 << nGpio);
    }
    else
    {
      Serial.println("Reset output");      
      slaveGpio &= ~(1 << nGpio);
    }
    
  }
  else
  {
   retVal= WS_ERROR;
  }
  //Serial.print("Pin =  ");
  //Serial.println(nGpio, HEX); 
  ///Serial.print("GPIO val =  ");
  //Serial.println(slaveGpio, HEX);
  return retVal;
}
// byte gpionumer + byte set/reset
WS_SINT32 writeDioAll( WS_UINT8 dataLen, WS_UINT8* pduData)
{
  WS_SINT32 retVal = WS_SUCCESS;
  WS_UINT8 nGpio =0 ;
  WS_UINT16  stas=0;
  WS_UINT16  *dtPtr = 0;
  nGpio = pduData[0];
  dtPtr = (WS_UINT16*)&pduData[1];
  stas = *dtPtr ;
  if(nGpio == 0xFF)
  {
    slaveGpio = stas;
  }
  else if(nGpio < 16) // max 16 gpio 0-15
  {
    if(stas & (1<nGpio) )
    {
      slaveGpio = slaveGpio & ~(1 << nGpio);
    }
    else
    {
      slaveGpio = slaveGpio & (1 << nGpio);
    }
  }  
  else
  {
   retVal= WS_ERROR;
  }

  Serial.print("Pin =  ");
  Serial.println(nGpio, HEX);
  Serial.print("val to Write=  ");
  Serial.println(*dtPtr, DEC);
  Serial.print("GPIO val =  ");
  Serial.println(slaveGpio, HEX);
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
      slaveGpio ^= (1 << nGpio);
    }
  }
  else
  {
   retVal= WS_ERROR;
  }
  return retVal;
}