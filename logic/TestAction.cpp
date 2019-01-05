#include "TestAction.h"
#include "../proto/pb/messageID.pb.h"
#include "../oryx/LogManager.h"
#include "../oryx/ClientConnManager.h"
#include "../oryx/MessageHandler.h"

using namespace Proto;

ActionResult TestAction::processMessage(Packet * packet, INT64 sessionID)
{
	if (!MESSAGE_ID_IsValid(packet->operatecode)) {
		return ActionResult::RESULT_PARSEERROR;
	}

	SessionConn * pConn = ClientConnManager::getInstance()->getSession_BySessionID(sessionID);
	if (pConn == NULL) {
		TRACEINFO("TestAction processMessage :%s from session:%ld (but no longer exist)", MESSAGE_ID_Name((MESSAGE_ID)packet->operatecode).c_str(), sessionID);
		return RESULT_SESSION_CLOSED;
	}
	else {
		TRACEINFO("TestAction processMessage :%s from session:%ld playerID:%ld", MESSAGE_ID_Name((MESSAGE_ID)packet->operatecode).c_str(), sessionID, pConn->player_id);
	}

	

	switch (packet->operatecode)
	{
	case MSGID_HEART_BEAT:
		break;
	default:
		break;
	}

	return ActionResult::RESULT_SUCCESS;
}

void TestAction::initAction()
{
	MessageHandler::getInstance()->registerHandlerFun(MSGID_HEART_BEAT, this);
}
