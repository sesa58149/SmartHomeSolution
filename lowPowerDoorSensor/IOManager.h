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

class ioInterface
{
  protected:
  int pinNo;
  int pinDir;
  int pinLastValue; 


  public:
  ioInterface()  {
     pinNo = PIN_UNDEF ;
     pinDir = PIN_DIR_UNDEF;
     pinLastValue = PIN_VAL_UNDEF;
  }
  ioInterface(int pinN, PIN_DIR dir){
    pinNo=pinN;
    pinDir = dir;
    pinLastValue = PIN_VAL_UNDEF;
  } 
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
  inputOutput(int pinNo, PIN_DIR dir):ioInterface(pinNo, dir){
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
