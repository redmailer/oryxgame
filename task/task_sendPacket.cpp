#include "task_sendPacket.h"


void task_sendPacket::on_thread_call()
{

}

void task_sendPacket::on_main_call()
{

}

void task_sendPacket::on_free()
{
	ORYX_DEL(this);
}
