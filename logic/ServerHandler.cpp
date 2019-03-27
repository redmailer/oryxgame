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
#include "RedisManager.h"


SINGLETON_DEFINE(ServerHandler)

ServerHandler::ServerHandler() {}

bool ServerHandler::init()
{
	if( !SINGLETON_INIT(LogManager) ){
		printf("SINGLETON_INIT(LogManager) failed\n");
		return false;
	}

	if( !SINGLETON_INIT(ConfigManager) ){
		TRACEERROR("SINGLETON_INIT(ConfigManager) failed");
		return false;
	}

	ConfigManager * cfgManager = ConfigManager::getInstance();
	LogManager * logManager = LogManager::getInstance();
	logManager->logic_Logger.init(cfgManager->log_game.path, cfgManager->log_game.log_level, cfgManager->print_screen);
	logManager->epoll_Logger.init(cfgManager->log_epoll.path, cfgManager->log_epoll.log_level, cfgManager->print_screen);
	logManager->async_Logger.init(cfgManager->log_async.path, cfgManager->log_async.log_level, cfgManager->print_screen);

	if(cfgManager->daemon_process){
		setDaemonProcess();
		signalIgnore();
	}

	if(!SINGLETON_INIT(RedisManager, cfgManager->redis_list)){
		TRACEERROR("SINGLETON_INIT(RedisManager) failed");
		return false;
	}

	if( !SINGLETON_INIT(PlayerManager) ){
		TRACEERROR("SINGLETON_INIT(PlayerManager) failed");
		return false;
	}
	SINGLETON_INIT(ClientConnManager);
	ACTION_REGISTER(TestAction);

	if( !SINGLETON_INIT(ThreadManager, cfgManager->io_thread_num) ){
		TRACEERROR("SINGLETON_INIT(ThreadManager) failed");
		return false;
	}

	TRACEGAME(LOG_LEVEL_INFO, "game start");

	Listener listen(cfgManager->listen_addr.c_str(), cfgManager->listen_port, DEVICE_SERVER_EXTERNAL);
	if (listen.doListen() == false) {
		return false;
	}

	ThreadManager::getInstance()->push_task(listen.CreateTask());

	TimerControl::register_timerFun(update_1s, 1000);
	TimerControl::register_timerFun(update_2s, 2000);
	TimerControl::register_timerFun(update_10s, 10000);

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

void ServerHandler::update_1s(INT64 time_stamp)
{
	//TRACEGAME(LOG_LEVEL_DEBUG,"ServerHandler::update_1s %ld", time_stamp);
	RedisManager * redisManager = RedisManager::getInstance();
	redisManager->checkRedisConnection();
}

void ServerHandler::update_2s(INT64 time_stamp)
{
	//TRACEGAME(LOG_LEVEL_DEBUG, "ServerHandler::update_2s %ld", time_stamp);
}

void ServerHandler::update_10s(INT64 time_stamp)
{
	//TRACEGAME(LOG_LEVEL_DEBUG, "ServerHandler::update_2s %ld", time_stamp);
}

