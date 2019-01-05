#ifndef __ACTION_TEST__
#define __ACTION_TEST__


#include "../oryx/Action.h"

class TestAction : public Action {
public:
	TestAction() {};

public:
	ActionResult processMessage(Packet * packet, INT64 sessionID);
	void initAction();
};


#endif



