#include "Listener.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include "LogManager.h"
using namespace std;

Listener::Listener()
{
    this->fd = -1;
    this->listen_addr = NULL;
    this->listen_port = 0;
    this->svr_type = DEVICE_UNKNOWN;
    BZERO(&svraddr, sizeof(svraddr));
}

Listener::Listener(const char *addr, INT32 port, INT32 svr_type)
{
    this->fd = -1;
    BZERO(&svraddr, sizeof(svraddr));
    this->listen_addr = addr;
    this->listen_port = port;
    this->svr_type = svr_type;
}

bool Listener::doListen()
{
    this->fd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->fd < 0)
    {
        TRACEERROR("CreateServer, socket fail:%d ,errno:%d", this->fd, errno);
        this->fd = -1;
    }

    setnonblocking(this->fd);
    setreuseaddr(this->fd);

    this->svraddr.sin_family = AF_INET;
    this->svraddr.sin_addr.s_addr = inet_addr(listen_addr);
    this->svraddr.sin_port = htons(listen_port);

    int ret = bind(this->fd, (sockaddr *)&svraddr, sizeof(svraddr));

    if (ret < 0)
    {
        TRACEERROR("CreateServer, bind fail:%d ,ret:%d, errno:%d", this->fd, ret, errno);
        return false;
    }

    ret = listen(this->fd, 4096);

    if (ret < 0)
    {
        TRACEERROR("CreateServer, listen fail fd :%d ,ret:%d,errno:%d", this->fd, ret, errno);
        return false;
    }

    TRACEINFO("CreateServer, listen at:%s:%d", listen_addr, listen_port);

    return true;
}
