#include "task_clientPacket.h"
#include "task_sendPacket.h"
#include "../oryx/threadManager.h"
#include "../oryx/LogManager.h"
#include "../oryx/MessageHandler.h"

using namespace std;

void task_clientConnPacket::on_thread_call()
{
	MessageHandler::getInstance()->processMessage(this);
}

void task_clientConnPacket::on_main_call()
{
	
}

void task_clientConnPacket::on_free()
{
	if (this->data_packet != NULL) {
		delete[] data_packet;
	}
	ORYX_DEL(this);
}
