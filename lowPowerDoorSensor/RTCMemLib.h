#ifndef __RTCMemLib__
#define __RTCMemLib__

//#define RTCMEMORYSTART 65

#define RTC_MAX_USR_MEM_SIZE  512
#define RTC_MAX_SYSTEM_MEM_SIZE 256
#define RTC_MAX_MEM_SIZE (RTC_MAX_USR_MEM_SIZE+RTC_MAX_SYSTEM_MEM_SIZE)
#define RTC_SYSTEM_MEM_START_ADD 0
#define RTC_USER_MEM_START_ADD ((RTC_MAX_SYSTEM_MEM_SIZE/4) +1) // RTC mem can only access in 4 butes group



extern "C" {
#include "user_interface.h"
}

// typedef struct {
//   int input;   
// } rtcStore;

class rtcUserMemAccess
{
  protected:
    int rtcUserMemStart;
  public: 
  rtcUserMemAccess()
  {
    rtcUserMemStart = RTC_USER_MEM_START_ADD;
  }
  void writeToRTCMemory(int index, int *writeBuf, int size_inBytes);
  void readFromRTCMemory(int index, int *readBuf, int size_inBytes); 
};


#endif //__RTCMemLib__