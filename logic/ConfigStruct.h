#ifndef __CONFIGSTRUCT_MANAGER__
#define __CONFIGSTRUCT_MANAGER__

#include "../oryx/common.h"
#include <vector>

struct Redis_Info {
    char addr[128];
    char auth[128];
    UINT16 port;

    Redis_Info(){
        BZERO(this, sizeof(Redis_Info));
    }
};
typedef std::vector<Redis_Info *> Redis_List;


struct Log_Info {
    char path[128];
    INT32 log_level;

    Log_Info(){
        BZERO(this, sizeof(Log_Info));
    }
};


#endif