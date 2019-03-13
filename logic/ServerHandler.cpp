#include "ServerHandler.h"

#include <stdio.h>
#include <unistd.h>   
#include <sys/ioctl.h>
#include <stdlib.h>
#include<sys/types.h> 
#include<sys/stat.h>

#include "../oryx/SignalTool.h"

#include "../oryx/Listener.h"
#include "../oryx/threadManager.h"
#include "../oryx/LogManager.h"
#include "../oryx/DeviceInfo.h"
#include "PlayerManager.h"
#include "TestAction.h"
#include "ConfigManager.h"



SINGLETON_DEFINE(ServerHandler)

ServerHandler::ServerHandler() {}

bool ServerHandler::init()
{
	setDaemonProcess();
	signalIgnore();

	if( !SINGLETON_INIT(LogManager) 
		|| !SINGLETON_INIT(ConfigManager) 
		|| !SINGLETON_INIT(ThreadManager)
		|| !SINGLETON_INIT(PlayerManager) ){
		return false
	}

	ACTION_REGISTER(TestAction);

	TRACEGAME(LOG_LEVEL_INFO, "game start");

	Listener listen("0.0.0.0", 8888, DEVICE_SERVER_EXTERNAL);
	if (listen.doListen() == false) {
		return false;
	}

	ThreadManager::getInstance()->push_task(listen.CreateTask());

	TimerControl::register_timerFun(update_1s, 1000);
	TimerControl::register_timerFun(update_2s, 2000);

	return true;
}

bool ServerHandler::signalIgnore()
{
	//����һЩ����Ҫ���ź�
	SignalTool::SignalIgnore(SIGINT);
	SignalTool::SignalIgnore(SIGHUP);
	SignalTool::SignalIgnore(SIGQUIT);
	SignalTool::SignalIgnore(SIGTTOU);
	SignalTool::SignalIgnore(SIGTTIN);
	SignalTool::SignalIgnore(SIGCHLD);
	SignalTool::SignalIgnore(SIGTERM);
	SignalTool::SignalIgnore(SIGHUP);
	SignalTool::SignalIgnore(SIGPIPE);
	return true;
}

bool ServerHandler::setDaemonProcess()
{
	pid_t pid;
	//�ø�������ֹ�����ӽ����ں�̨����
	if ((pid = fork()) != 0)
	{
		exit(0);
	}
	
	//ʹ�ӽ��̳�Ϊ�Ự�鳤�����ն�����
	setsid();
	
	//�ûỰ�鳤��ֹ���õڶ��ӽ������У���ֹ�����ն�
	if ((pid = fork()) != 0)
	{
		exit(0);
	}

	umask(0);

	return true;
}

void ServerHandler::run()
{
	while (true)
	{
		INT32 timerNums = TimerControl::update();
		INT32 threadNums = ThreadManager::getInstance()->update();

		if (threadNums <= 0 && timerNums <= 0) {
			usleep(10000);
		}
		else {
			//cout << "---mainhandler tasks:" << nums << " at time :" << GetCourrentTime() << endl;
		}
	}
}

void ServerHandler::update_1s(INT64 time_MilliSec)
{
	//TRACEGAME(LOG_LEVEL_DEBUG,"ServerHandler::update_1s %ld", time_MilliSec);
}

void ServerHandler::update_2s(INT64 time_MilliSec)
{
	//TRACEGAME(LOG_LEVEL_DEBUG, "ServerHandler::update_2s %ld", time_MilliSec);
}

