#include<Arduino.h>
#include"IOManager.h"

int ioInterface::pinRead()
{
  return digitalRead(pinNo);
}

int ioInterface::pinWrite( int val)
{
  int retVal =-1;
  if(pinDir == PIN_DIR_OUT)
  {
      digitalWrite(pinNo, val);
      retVal=  0;
  }
  return retVal;
}
int ioInterface::getLastVal()
{
  return pinLastValue;
}

int ioInterface::setLastVal(int val)
{
  int retVal = -1;
  if(val == 0 || val == 1)
  {
    pinLastValue = val;
    retVal = 0;
  }
  return retVal;
}


/**********************************************************************************/

int inputOutput::saveLastVal()
{
  writeToRTCMemory(objectId, &pinLastValue, sizeof(int));
  return 0;
} 

int inputOutput::loadLastVal()
{
  int val =0;
  readFromRTCMemory(objectId, &val, sizeof(int));
  pinLastValue = val;
  Serial.print("RTC read Val    ");
  Serial.println(pinLastValue);
  return 0;
} 

bool inputOutput::isInputChanged()
{
  bool retVal = false;
  
  int pinVal = pinRead();
  Serial.print( "Input pin val    ");
  Serial.println(pinVal);
  if(pinVal != pinLastValue) 
  {
    pinLastValue = pinVal;
    saveLastVal();
    retVal = true;
  }
  return retVal;
}




