#include "ClientConnManager.h"
#include "LogManager.h"
#include "../task/task_sendPacket.h"

SINGLETON_DEFINE(ClientConnManager)

ClientConnManager::ClientConnManager() {}

bool ClientConnManager::init()
{
    m_iAutoIncriSessionID = 1L;
    return true;
}

bool ClientConnManager::addNewConn(INT64 threadID, INT64 sessionID, DEVICE_TYPE deviceType, struct sockaddr_in &addr)
{
    if (getSession_BySessionID(sessionID) != NULL)
    {
        TRACEWARN("add session repeated:%ld", sessionID);
        return false;
    }
    TRACEWARN("add session success:%ld", sessionID);
    SessionConn *sessionConn = ORYX_NEW(SessionConn, threadID, sessionID, deviceType, addr);
    m_MapSessionConnInfo[sessionID] = sessionConn;
    onSessionBegin(sessionConn);
    return true;
}

void ClientConnManager::closeConn(INT64 sessionID)
{
    SessionConn *sessionConn = getSession_BySessionID(sessionID);
    if (sessionConn == NULL)
    {
        return;
    }

    task_clientConnDelete *pClientConnDel = ORYX_NEW(task_clientConnDelete, sessionConn->thread_id, sessionConn->session_id);
    ThreadManager::getInstance()->push_task(pClientConnDel);
    removeSessionConn(sessionID);
}

void ClientConnManager::onSessionBegin(SessionConn *pConn)
{
}

void ClientConnManager::onSessionClose(SessionConn *pConn)
{
}

void ClientConnManager::removeSessionConn(INT64 sessionID)
{
    CONN_INFO_MAP::iterator it = m_MapSessionConnInfo.find(sessionID);
    if (it != m_MapSessionConnInfo.end())
    {
        TRACEINFO("removeSessionConn:%ld", sessionID);
        ORYX_DEL(it->second);
        onSessionClose(it->second);
        m_MapSessionConnInfo.erase(it);
    }
}

INT64 ClientConnManager::getRandNewSession()
{
    INT64 sessionID_rand = 0;
    while (true)
    {
        sessionID_rand = GetCourrentTime() * 100000L + m_iAutoIncriSessionID;
        m_iAutoIncriSessionID = m_iAutoIncriSessionID % 100000L + 1;

        if (m_MapSessionConnInfo.find(sessionID_rand) == m_MapSessionConnInfo.end())
        {
            break;
        }
    }
    return sessionID_rand;
}

bool ClientConnManager::sendMsgToConn(INT64 sessionID, void *data, INT32 len)
{
    if (data == NULL || len <= 0)
    {
        return false;
    }

    SessionConn *sessionConn = getSession_BySessionID(sessionID);
    if (sessionConn == NULL)
    {
        return false;
    }

    task_sendPacket *sendPacket = ORYX_NEW(task_sendPacket, sessionConn->thread_id, sessionID);

    if (sendPacket->InitData(data, len) == false)
    {
        return false;
    }

    ThreadManager::getInstance()->push_task(sendPacket);

    return true;
}
