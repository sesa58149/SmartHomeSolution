#include "KHomeTask.h"
ws_TCB *wsTask = 0;
int tcbLen=0;
void taskScheduler()
{
  int i = 0;
  for(i=0; i<tcbLen; i++)
  {
    if( (wsTask[i].isStarted == 1) )
    {
      wsTask[i].periodExpire++;
      if(wsTask[i].periodExpire == wsTask[i].period_ms)
      {
        wsTask[i].periodExpire=0;
        wsTask[i].isRunnable = 1;
      }
    }
  }
}
void taskRunner()
{
  int i = 0;
  for(i=0; i<tcbLen; i++)
  {
    if( (wsTask[i].isStarted == 1) && (wsTask[i].isRunnable == 1) )
    {
      wsTask[i].isRunnable = 0;
      wsTask[i].task();
    }
  }
}

void taskResume(int id)
{
  wsTask[id].isStarted = 1; 
}
void taskSuspand(int id)
{
  wsTask[id].isStarted = 0; 
}
WS_UINT8 registerTasks(ws_TCB * tcb, WS_UINT8 nTask)
{
  if(tcb == NULL)
    return WS_ERROR;
    
  wsTask = tcb;
  tcbLen = nTask;
  return WS_SUCCESS;  
}
void startTask()
{
  int i = 0;
  
  for(i=0; i<tcbLen; i++)
  {
    wsTask[i].isStarted = 1;   
  } 
  
}
