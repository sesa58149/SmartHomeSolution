#ifndef KHOMETASK_H__
  #define KHOMETASK_H__

#include "userDef.h"
typedef void (*taskHandler)(void);
typedef struct 
{
  WS_UINT16 taskId;
  taskHandler task;  
  WS_UINT32 period_ms;
  WS_UINT32 periodExpire;
  WS_UINT8 isRunnable;
  WS_UINT8 isStarted;
}ws_TCB;

#define TIME_TICK 10 // mSec

void taskRunner();
void taskResume(int id);
void taskSuspand(int id);
WS_UINT8 registerTasks(ws_TCB * tcb, WS_UINT8 nTask);
void startTask();
void taskScheduler();

#endif// KHOMETASK_H__
