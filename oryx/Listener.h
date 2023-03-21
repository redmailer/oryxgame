#ifndef __LISTENER__
#define __LISTENER__

#include "common.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "../task/task_listener.h"

class Listener
{
public:
    Listener();
    Listener(const char *addr, INT32 port, INT32 svr_type);

    bool doListen();

    inline task_listener *CreateTask()
    {
        if (this->fd < 0)
        {
            return NULL;
        }
        task_listener *pTask = ORYX_NEW(task_listener, this->fd, (DEVICE_TYPE)this->svr_type, this->svraddr);
        return pTask;
    }

private:
    INT32 fd;
    const char *listen_addr;
    INT32 listen_port;
    INT32 svr_type;
    sockaddr_in svraddr;
};

#endif