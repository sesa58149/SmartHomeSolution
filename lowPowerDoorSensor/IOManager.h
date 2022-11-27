#include "HardwareSerial.h"
#include <type_traits>
#ifndef _INOUT_OP__
#define _INOUT_OP__

#include "RTCMemLib.h"

typedef enum
{
  PIN_DIR_UNDEF =0,
  PIN_DIR_IN,
  PIN_DIR_OUT
}PIN_DIR;

#define PIN_UNDEF 0xFF
#define PIN_VAL_UNDEF 0xFF

typedef struct
{
  int adcBits;
  float vccRef;
}analogProperties;

class ioInterface
{
  protected:
  int pinNo;
  int pinDir;
  int pinLastValue; 
  analogProperties acdProp;
  float adcFactor;
  char name[32];


  public:
  ioInterface()  {
     pinNo = PIN_UNDEF ;
     pinDir = PIN_DIR_UNDEF;
     pinLastValue = PIN_VAL_UNDEF;
     acdProp={0,0};
     memcpy((void*) &name[0], "sensor_name", sizeof("sensor_name"));
  }
  ioInterface(char* pinName, int pinN, PIN_DIR dir){
    pinNo=pinN;
    pinDir = dir;
    if( pinDir ==  PIN_DIR_OUT)
    {
      pinMode(pinN, OUTPUT);
    }
    pinLastValue = PIN_VAL_UNDEF;
    acdProp = {0,0};
    adcFactor = (float) PIN_VAL_UNDEF;
    memcpy((void*) &name[0], (void*)pinName, strlen(pinName));
    
  } 

  char* getName()
  {       
    return name;
  }

  int setAdcProperty(analogProperties *adcP);
  float getAdcPinVal();

  int pinRead();
  int pinWrite( int val);
  int getLastVal();
  virtual int saveLastVal() 
  {
    return -1;

  } //virtual function
  virtual int loadLastVal()
  {
    return -1;
  } //virtual function
  int setLastVal(int val);  
};


class inputOutput:public ioInterface, public rtcUserMemAccess
{
  protected:
    int objectId;

  public:
  inputOutput()  {
  }
  inputOutput(char*pinName, int pinNo, PIN_DIR dir):ioInterface(pinName, pinNo, dir){
   objectId = objectCnt++;
  }
  int getObjId()
  {
    return objectId;
  }
  static int objectCnt;

  int saveLastVal(); 
  int loadLastVal(); 
  bool isInputChanged();
};




#endif
