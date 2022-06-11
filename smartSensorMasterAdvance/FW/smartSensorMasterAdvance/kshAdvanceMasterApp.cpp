#include "kshAdvanceMasterApp.h"
#include "kshEspNowPdu.h"
#include "kshDeviceInfo.h"

SLAVE_DEVICE_INFO deviceList[MAX_LIST_SIZE];

void kshDicoverEspNowNetworok()
{
    
slaveSendWhoIs();

}

// find the list if already added then reject or add to the list upto LIST_MAX_SIZE
// compaire the device mac add for uniqness
void addDeviceInfoToList(SLAVE_DEVICE_INFO *devInfo)
{

}