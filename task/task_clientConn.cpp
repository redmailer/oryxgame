#include "task_clientConn.h"
#include "../oryx/threadManager.h"
#include "../oryx/ClientConnManager.h"

void task_clientConn::on_thread_call()
{
    task_clientConnInsert *pClientConnInsert = ORYX_NEW(task_clientConnInsert, this->clientFD, this->device_type, this->sock_addr);
    pClientConnInsert->session_id = ClientConnManager::getInstance()->getRandNewSession();

    INT64 threadid = ThreadManager::getInstance()->push_task(pClientConnInsert);
    if (threadid > 0)
    {
        ClientConnManager::getInstance()->addNewConn(threadid, pClientConnInsert->session_id, this->device_type, this->sock_addr);
    }
}

void task_clientConn::on_main_call()
{
}

void task_clientConn::on_free()
{
    ORYX_DEL(this);
}

//-------------------------------------------------------------------------------------------------//

void task_clientConnInsert::on_thread_call()
{
}

void task_clientConnInsert::on_main_call()
{
}

void task_clientConnInsert::on_free()
{
    ORYX_DEL(this);
}

//-------------------------------------------------------------------------------------------------//

void task_clientConnOnClose::on_thread_call()
{
    ClientConnManager::getInstance()->removeSessionConn(session_id);
}

void task_clientConnOnClose::on_main_call()
{
}

void task_clientConnOnClose::on_free()
{
    ORYX_DEL(this);
}

//-------------------------------------------------------------------------------------------------//

void task_clientConnDelete::on_thread_call()
{
}

void task_clientConnDelete::on_main_call()
{
}

void task_clientConnDelete::on_free()
{
    ORYX_DEL(this);
}
