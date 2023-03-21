#ifndef __ACTION_LOGIN__
#define __ACTION_LOGIN__

#include "../oryx/Action.h"

class LoginAction : public Action
{
public:
    LoginAction(){};

public:
    ActionResult processMessage(Packet *packet, INT64 sessionID);
    void initAction();
};

#endif
