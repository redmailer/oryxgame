#ifndef __MESSAGE_HANDLER__
#define __MESSAGE_HANDLER__

#include <iostream>
#include "common.h"
#include <unordered_map>
#include "Action.h"
#include "../proto/pb/messageStruct.pb.h"

using namespace std;

typedef unordered_map<INT32, Action *> MessageHandlerMap;

class MessageHandler
{
    SINGLETON_DECLEAR(MessageHandler)

public:
    bool init();

    inline bool registerHandlerFun(INT32 messageID, Action *act)
    {
        if (act == NULL)
        {
            return false;
        }
        m_mapFun[messageID] = act;
        return true;
    }

    ActionResult processMessage(void *paramMessage);

private:
    MessageHandlerMap m_mapFun;
};

#endif
