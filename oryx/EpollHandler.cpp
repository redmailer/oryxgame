#include "EpollHandler.h"
#include <iostream>
#include <stdio.h>
#include "../task/task_listener.h"
#include "../task/task_clientConn.h"
#include "../task/task_clientPacket.h"
#include "../task/task_sendPacket.h"

#include <errno.h>
#include "DeviceInfo.h"


using namespace std;


EpollHandler::EpollHandler():Thread(EpollHandler::epoll_thread_run,this)
{
	this->epfd = -1;
	this->thread_id = -1;
}

void * EpollHandler::epoll_thread_run(void * param)
{
	EpollHandler * epollHandler = (EpollHandler *)param;
	epollHandler->Init();

	while (true) {
		INT32 eventNums = epollHandler->EpollRun();

		eventNums += epollHandler->update_thread();

		if (eventNums <= 0) {
			usleep(10000);
		}
		else {
			//cout << "+++epollthread " << epollHandler->getThreadID() << " events num :" << eventNums << " at time:" << GetCourrentTime() << endl;
		}
	}
	return NULL;
}

void EpollHandler::dotask(tagThreadTaskNode & node)
{
	TASK_TYPE taskType = node.pGetTaskType(node.pTask);
	switch (taskType) {
		case TASK_TYPE_LISTENER:
			{
				AddListener(node);
			}
			break;
		case TASK_TYPE_CLIENTCONN_INSERT:
			{
				AddClientConn(node);
			}
			break;
		case TASK_TYPE_CLIENTCONN_DELETE:
			{
				DelClientConn(node);
			}
			break;
		case TASK_TYPE_SEND_PACKET:
			{
				WritePacketToConn(node);
			}
			break;
		default:
			break;
		
	}
	Thread::dotask(node);
}

void EpollHandler::dotask_complete(tagThreadTaskNode & node)
{
	Thread::dotask_complete(node);
}

bool EpollHandler::Init()
{
	return this->CreateEpoll();
}

bool EpollHandler::CreateEpoll()
{
	this->epfd = epoll_create(4096);
	if (epfd < 0)
	{
		TRACEERROR("epoll_create fail:%d errno:%d  at thread %ld", epfd, errno, thread_id);
		epfd = -1;

		return false;
	}
	TRACEINFO("epoll_create :%d  at thread %ld", epfd, thread_id);
	return true;
}

bool EpollHandler::EpollRegister(INT32 fd, INT32 eventtype /*= EPOLLIN | EPOLLET*/)
{
	struct epoll_event ev;              //�¼���ʱ����
	ev.data.fd = fd;
	ev.events = eventtype;				//EPOLLET ��Ե    EPOLLLTˮƽ

	setnonblocking(fd);

	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

	if (ret != 0)
	{
		TRACEEPOLL(LOG_LEVEL_ERROR, "epoll_ctl add fail:%d ,errno:%d  ,fd:%d", ret, errno, fd);
		return false;
	}

	return true;
}

void EpollHandler::EpollRemove(INT32 fd)
{
	struct epoll_event ev;              //�¼���ʱ����
	ev.data.fd = fd;

	int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);

	if (ret != 0)
	{
		TRACEEPOLL(LOG_LEVEL_ERROR, "epoll_ctl remove fail:%d ,errno:%d  ,fd:", ret, errno, fd);
	}
	return;
}


INT32 EpollHandler::EpollRun(INT32 maxEvent) {
	if (maxEvent <= 0) {
		return -1;
	}

	if (maxEvent > MAXEVENTS) {
		maxEvent = MAXEVENTS;
	}
	BZERO(events_slot, sizeof(events_slot));
	INT32 nfds = epoll_wait(epfd, events_slot, MAXEVENTS, 0);

	if (nfds <= 0) {
		return nfds;
	}
	doEpollEvents(nfds);

	return nfds > maxEvent ? maxEvent : nfds;
}

void EpollHandler::AddListener(tagThreadTaskNode & node)
{
	task_listener * pListener = (task_listener *)node.pTask;

	bool result = this->EpollRegister(
		pListener->listenFD,
		EPOLLIN | EPOLLET);

	if (result == true) {
		addDevice(pListener->listenFD, 0, pListener->device_type,pListener->sock_addr);
	}
}

void EpollHandler::AddClientConn(tagThreadTaskNode & node)
{
	task_clientConnInsert * pClientConn = (task_clientConnInsert *)node.pTask;
	pClientConn->m_thrdID = getThreadID();
	bool result = this->EpollRegister(
		pClientConn->clientFD,
		EPOLLIN | EPOLLOUT | EPOLLET );

	if (result == true) {
		addDevice(pClientConn->clientFD, pClientConn->session_id, pClientConn->device_type, pClientConn->sock_addr);
	}
}

void EpollHandler::DelClientConn(tagThreadTaskNode & node)
{
	task_clientConnDelete * pConnDel = (task_clientConnDelete *)node.pTask;
	removeDevice_BySession(pConnDel->session_id);
}

void EpollHandler::WritePacketToConn(tagThreadTaskNode & node)
{
	task_sendPacket * pSend = (task_sendPacket *)node.pTask;
	DEVICE_INFO * deviceInfo = GetDeviceInfo_BySesion(pSend->session_id);
	if (deviceInfo == NULL) {
		TRACEERROR("send failed session_id:%ld", pSend->session_id);
		return;
	}

	bool result = true;
	while(true){
		result = doWrite(deviceInfo->fd);
		if (!result){
			break;
		}

		INT32 sendlen = deviceInfo->write(pSend->data_packet, pSend->data_size);

		if (sendlen < pSend->data_size) {
			TRACEEPOLL(LOG_LEVEL_WARN, "send failed session:%ld fd:%d", pSend->session_id, deviceInfo->fd);
			result = false;
			break;
		}

		result = doWrite(deviceInfo->fd);
		break;
	}

	//����ʧ�ܣ��ϵ�socket
	if (result == false) {
		task_clientConnOnClose * pConnClose = ORYX_NEW( task_clientConnOnClose,getThreadID(), deviceInfo->session_id);
		push_task_main(pConnClose);
		removeDevice_ByFD(deviceInfo->fd);
	}

	return;
}

bool EpollHandler::addDevice(INT32 fd, INT64 sessionID, DEVICE_TYPE deviceType, struct sockaddr_in& sock_addr)
{
	if (GetDeviceInfo_BySesion(sessionID) != NULL || GetDeviceInfo_ByFD(fd) != NULL) {
		TRACEEPOLL(LOG_LEVEL_ERROR, "addDevice falied fd:%d sessionID:%ld deviceType:%d IP:%s,PORT:%d",
			fd, sessionID, deviceType, inet_ntoa(sock_addr.sin_addr), ntohs(sock_addr.sin_port));
		return false;
	}

	DEVICE_INFO * deviceInfo = ORYX_NEW(DEVICE_INFO, deviceType);
	deviceInfo->fd = fd;
	deviceInfo->session_id = sessionID;
	memcpy(&deviceInfo->sock_addr, &sock_addr, sizeof(sock_addr));

	m_MapDeviceInfo[deviceInfo->fd] = deviceInfo;
	m_MapDeviceSessionInfo[deviceInfo->session_id] = deviceInfo;
	TRACEEPOLL(LOG_LEVEL_INFO, "addDevice thread:%ld fd:%d sessionID:%ld deviceType:%d IP:%s,PORT:%d",
		getThreadID() ,fd, sessionID, deviceType,inet_ntoa(deviceInfo->sock_addr.sin_addr),ntohs(deviceInfo->sock_addr.sin_port));

	addPressure(1);
	return true;
}

void EpollHandler::removeDevice_BySession(INT64 sessionID)
{
	DEVICE_INFO * pDevice = GetDeviceInfo_BySesion(sessionID);
	if (pDevice != NULL) {
		TRACEEPOLL(LOG_LEVEL_INFO, "removeDevice thread:%ld fd:%d sessionID:%ld deviceType:%d",
			getThreadID(), pDevice->fd, pDevice->session_id, pDevice->device_type);
		close(pDevice->fd);
		EpollRemove(pDevice->fd);
		addPressure(-1);
		m_MapDeviceInfo.erase(pDevice->fd);
		m_MapDeviceSessionInfo.erase(pDevice->session_id);
		ORYX_DEL(pDevice);
	}
}

void EpollHandler::removeDevice_ByFD(INT32 fd)
{
	DEVICE_INFO * pDevice = GetDeviceInfo_ByFD(fd);
	if (pDevice != NULL) {
		TRACEEPOLL(LOG_LEVEL_INFO, "removeDevice thread:%ld fd:%d sessionID:%ld deviceType:%d",
			getThreadID(), pDevice->fd, pDevice->session_id, pDevice->device_type);
		EpollRemove(pDevice->fd);
		close(pDevice->fd);
		addPressure(-1);
		m_MapDeviceInfo.erase(pDevice->fd);
		m_MapDeviceSessionInfo.erase(pDevice->session_id);
		ORYX_DEL(pDevice);
	}
}

void EpollHandler::doEpollEvents(INT32 nfds)
{
	for (int i = 0; i < nfds && i < MAXEVENTS; ++i) {
		INT32 fd_temp = events_slot[i].data.fd;
		bool eventResult = true;
		switch (GetDeviceType_ByFD(fd_temp))
		{
			case DEVICE_SERVER_INTERNAL:
			case DEVICE_SERVER_EXTERNAL:
				doAccept(fd_temp);
				break;
			case DEVICE_CLIENT_INTERNAL:
			case DEVICE_CLIENT_EXTERNAL:
				if (events_slot[i].events & EPOLLOUT) {
					eventResult = doWrite(fd_temp);
					if (eventResult == false) {
						break;
					}
					
				}
				if(events_slot[i].events & EPOLLIN)
				{
					eventResult = doRead(fd_temp);
					if (eventResult == false) {
						break;
					}
					
				}
				break;
			case DEVICE_UNKNOWN:
				
			default:
				break;
		}

		if (eventResult == false) {
			DEVICE_INFO * pDevice = GetDeviceInfo_ByFD(fd_temp);
			if (pDevice) {
				task_clientConnOnClose * pConnClose = ORYX_NEW(task_clientConnOnClose,getThreadID(), pDevice->session_id);
				push_task_main(pConnClose);
			}
			removeDevice_ByFD(fd_temp);
			
		}
	}
}

bool EpollHandler::doAccept(INT32 fd)
{
	DEVICE_INFO * deviceInfo = GetDeviceInfo_ByFD(fd);
	if (deviceInfo == NULL) {
		return false;
	}

	socklen_t lenth = sizeof(sockaddr_in);
	struct sockaddr_in clientaddr;

	while (true) {
		memset(&clientaddr, 0, sizeof(clientaddr));
		int connectfd = accept(fd, (sockaddr *)&clientaddr, &lenth);
		if (connectfd < 0) {
			break;
		}

		setnonblocking(connectfd);

		DEVICE_TYPE clientDeviceType = DEVICE_INFO::getClientDeviceType(deviceInfo->device_type);
		task_clientConn * pClientConn = ORYX_NEW(task_clientConn, getThreadID(), connectfd, clientDeviceType, clientaddr);

		this->push_task_main(pClientConn);
	}

	return true;

}

bool EpollHandler::doRead(INT32 fd)
{
	
	bool result = true;
	INT32 recvNum = 0;
	DEVICE_INFO * deviceInfo = GetDeviceInfo_ByFD(fd);
	if (deviceInfo == NULL) {
		return false;
	}

	//int retry = 0;

	do {
		//���ݻ���ǰ�ƣ��ճ������session������
		deviceInfo->clear_Recv_buff();

		recvNum = ::recv(
			deviceInfo->fd,
			deviceInfo->recv_buffer + deviceInfo->recv_end,
			MAX_RECV_BUFFER_LEN - deviceInfo->recv_end,
			0);

		if (recvNum < 0) {
			INT32 iErrorNo = errno;
			if (iErrorNo == EAGAIN) {		//
				break;
			}
			else if (iErrorNo == EWOULDBLOCK) {	//
				break;
			}
			else if (iErrorNo == EINTR) {	//������
				break;
			}
			else {
				result = false;
				break;
			}
			
		}
		else if (recvNum == 0) {			//������
			result = false;
			break;
		}
		else {
			deviceInfo->recv_end += recvNum;
			//ǰ�ĸ��ֽڴ����������ĳ��ȣ�������4���ֽ��ٿ�ʼ���
			while (deviceInfo->recv_end - deviceInfo->recv_begin > 4 ) {
				INT32 * temp = (INT32 *)(deviceInfo->recv_buffer + deviceInfo->recv_begin);
				INT32 packetLen = ntohl(*temp);

				//��̫�� ���� ���ȴ��󣬲����ˣ�ֱ�ӶϿ�
				if (packetLen > MAX_RECV_BUFFER_LEN || packetLen < 4) {
					TRACEEPOLL(LOG_LEVEL_ERROR, "session:%ld-----doRead-----packetLen:%d illegal", deviceInfo->session_id, packetLen);
					result = false;
					break;
				}

				//һ����û�������������������
				if (packetLen > deviceInfo->recv_end - deviceInfo->recv_begin) {
					break;
				}

				//�չ�һ������ֱ�ӽ�������
				//Packet* packet = Packet::NewPacket(deviceInfo->recv_buffer + deviceInfo->recv_begin, packetLen);
				task_clientConnPacket * pPacket = ORYX_NEW(task_clientConnPacket,getThreadID(), deviceInfo->session_id);
				pPacket->InitData(deviceInfo->recv_buffer + deviceInfo->recv_begin , packetLen );
				push_task_main(pPacket);

				deviceInfo->recv_begin += packetLen;
			}
		}

	} while (result);

	return result;
}

bool EpollHandler::doWrite(INT32 fd)
{
	DEVICE_INFO * deviceInfo = GetDeviceInfo_ByFD(fd);
	if (deviceInfo == NULL) {
		return false;
	}

	if (deviceInfo->send_end <= deviceInfo->send_begin) {
		return true;
	}

	bool result = true;

	INT32 sendLen = 0;
	do {
		deviceInfo->clear_Send_buff();

		if (deviceInfo->send_end <= deviceInfo->send_begin) {
			break;
		}

		sendLen = send(deviceInfo->fd,
			deviceInfo->send_buffer + deviceInfo->send_begin,
			deviceInfo->send_end - deviceInfo->send_begin,
			0);

		if (sendLen < 0) {
			INT32 iErrorNo = errno;
			if (iErrorNo == EAGAIN) {		//���·�
				break;
			}
			else if (iErrorNo == EWOULDBLOCK) {
				break;
			}
			else if (iErrorNo == EINTR) {	//������
				break;
			}
			else {
				result = false;
				break;
			}

		}
		else if (sendLen == 0) {
			result = false;
			break;
		}
		TRACEEPOLL(LOG_LEVEL_INFO, "send to %ld  len:%ld  at thread:%ld", deviceInfo->session_id, sendLen, getThreadID());
		deviceInfo->send_begin += sendLen;

	} while (result);

	return result;
}

