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



SINGLETON_DEFINE(ServerHandler)

ServerHandler::ServerHandler() {}

bool ServerHandler::init()
{
	setDaemonProcess();
	signalIgnore();

	SINGLETON_INIT(LogManager);
	SINGLETON_INIT(ThreadManager);
	SINGLETON_INIT(PlayerManager);

	ACTION_REGISTER(TestAction);

	TRACEGAME(LOG_LEVEL_INFO, "game start");

	Listener listen("0.0.0.0", 8888, DEVICE_SERVER_EXTERNAL);
	if (listen.doListen() == false) {
		return 0;
	}

	ThreadManager::getInstance()->push_task(listen.CreateTask());

	TimerControl::register_timerFun(update_1s, 1000);
	TimerControl::register_timerFun(update_2s, 2000);

	return true;
}

bool ServerHandler::signalIgnore()
{
	//忽略一些不必要的信号
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
	//让父进程终止，让子进程在后台运行
	if ((pid = fork()) != 0)
	{
		exit(0);
	}
	
	//使子进程成为会话组长，与终端脱离
	setsid();
	
	//让会话组长终止，用第二子进程运行，防止打开新终端
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
			usleep(2000);
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

