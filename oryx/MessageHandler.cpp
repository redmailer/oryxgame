#include "MessageHandler.h"
#include "../task/task_clientPacket.h"
#include "packet.h"
#include "LogManager.h"

SINGLETON_DEFINE(MessageHandler)

MessageHandler::MessageHandler(){};

bool MessageHandler::init()
{
	return true;
}

ActionResult MessageHandler::processMessage(void * paramMessage)
{
	task_clientConnPacket * pConnMsg = (task_clientConnPacket *)paramMessage;
	if (pConnMsg == NULL || pConnMsg->data_packet == NULL) {
		TRACEERROR("(pConnMsg == NULL || pConnMsg->data_packet == NULL)");
		return RESULT_PARSEERROR;
	}

	Packet * pPacket = Packet::NewPacketFromBytes(pConnMsg->data_packet, pConnMsg->data_size);
	if (pPacket == NULL) {
		TRACEERROR("NewPacketFromBytes failed session_id:%ld", pConnMsg->session_id);
		return RESULT_PARSEERROR;
	}

	MessageHandlerMap::iterator it_action = m_mapFun.find(pPacket->operatecode);
	if (it_action == m_mapFun.end() || it_action->second == NULL) {
		TRACEWARN("cannot find messagefun, messageid :%d from session:%ld", pPacket->operatecode, pConnMsg->session_id);
		ORYX_DEL(pPacket);
		return RESULT_UNKNOWNMESSAGE;
	}

	if (it_action->second == NULL) {
		m_mapFun.erase(it_action);
		TRACEWARN("messagefun is NULL :%d", pPacket->operatecode);
		return RESULT_UNKNOWNMESSAGE;
	}

	Action * action = it_action->second;
	TRACEDEBUG("processMessage debug messageid :%d from session:%ld, readDataLen:%d",pPacket->operatecode, pConnMsg->session_id, pPacket->GetRealDataLen());
	ActionResult result = action->processMessage(*pPacket, pConnMsg->session_id);

	//������첽�ȴ��У���ɾ��
	if (result != RESULT_ASYNC_WAIT) {
		ORYX_DEL(pPacket);
	}

	if (result != RESULT_SUCCESS && result != RESULT_ASYNC_WAIT) {
		TRACEERROR("processMessage failed:%d,messageid :%d from session:%ld", result, pPacket->operatecode, pConnMsg->session_id);
	}

	return result;
}

