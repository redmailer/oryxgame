#ifndef __Action__
#define __Action__

#include "common.h"
#include "packet.h"

enum ActionResult {
	RESULT_SUCCESS = 0,
	RESULT_ASYNC_WAIT = 1,
	RESULT_UNKNOWNMESSAGE = 2,
	RESULT_PARSEERROR = 3,
	RESULT_SESSION_CLOSED = 4,
    RESULT_MAX,
};

#define ACTION_REGISTER(ActionType)  ActionType * act = new ActionType();act->initAction();

class Action {
public:
	Action() {};

	virtual ~Action() {};

	virtual void initAction() = 0;

	virtual ActionResult processMessage(Packet * packet, INT64 sessionID) = 0;

};

#endif
