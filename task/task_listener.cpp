#include "task_listener.h"
using namespace std;
void task_listener::on_thread_call()
{

}

void task_listener::on_main_call()
{

}

void task_listener::on_free()
{
	ORYX_DEL(this);
}
