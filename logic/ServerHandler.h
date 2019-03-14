#ifndef __SERVER_HANDLER__
#define __SERVER_HANDLER__

#include "../oryx/common.h"
#include "../oryx/TimerControl.h"

class ServerHandler : TimerControl {
	SINGLETON_DECLEAR(ServerHandler)
public:

	bool init();

	bool signalIgnore();

	bool setDaemonProcess();

	void run();

	
public :
	static void update_1s(INT64 time_stamp);
	static void update_2s(INT64 time_stamp);
	static void update_10s(INT64 time_stamp);

};

#endif
