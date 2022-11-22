#include "Arduino.h"
#include"RTCMemLib.h"

// // nByte must be group  of 4 btes
// void readFromRTCMemory(rtcStore * rtcData, int nByte) {
//   system_rtc_mem_read(RTCMEMORYSTART, rtcData, nByte);

// }

// void writeToRTCMemory(rtcStore * rtcData, int nByte) {
  
//   Serial.println(sizeof(rtcStore));
//   system_rtc_mem_write(RTCMEMORYSTART, rtcData, nByte);

// }


void rtcUserMemAccess:: writeToRTCMemory(int index, int *writeBuf, int size_inBytes)
{
  system_rtc_mem_write( (rtcUserMemStart+index), writeBuf, size_inBytes);
  Serial.print( "RTC start Add   ");
  Serial.println(rtcUserMemStart+index);
}
void rtcUserMemAccess:: readFromRTCMemory(int index, int *readBuf, int size_inBytes)
{
  system_rtc_mem_read( (rtcUserMemStart+index), readBuf, size_inBytes);
  Serial.print( "RTC start Add   ");
  Serial.println(rtcUserMemStart+index);
} 