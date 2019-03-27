#include "PlayerManager.h"
#include "../proto/pb/messageID.pb.h"
#include "../oryx/packet.h"
PlayerManager::PlayerManager() {};



bool PlayerManager::init()
{
	m_iAutoIncrPlayerID = 1001;
	ClientConnManager::init();
	return true;
}

void PlayerManager::onSessionClose(SessionConn * pConn)
{
	if (pConn && pConn->player_id > 0) {
		onPlayerLogoff(pConn->player_id);
	}
	return;
}

INT32 PlayerManager::onPlayerLogin(INT64 player_id, SessionConn * pConn)
{
	if (player_id <= 0 || pConn == NULL) {
		TRACEERROR("onPlayerLogin failed player:%ld", player_id);
		return Proto::RESULT_LOGIN_NOEXIST;
	}

	if (pConn->player_id == player_id) {
		TRACEWARN("onPlayerLogin repeated player:%ld", player_id);
		return Proto::RESULT_LOGIN_REPEATED;
	}

	Player * pPlayer = getPlayer(player_id);
	if (pPlayer == NULL){
		TRACEERROR("onPlayerLogin failed cannot find player:%ld ", player_id);
		return Proto::RESULT_LOGIN_NOEXIST;
	}

	INT64 oldSession_id = 0;
	SessionConn * pOldConn = getPlayerSession(player_id);
	if (pOldConn != NULL) {
		pOldConn->player_id = 0;
		oldSession_id = pOldConn->session_id;
	}
	pConn->player_id = player_id;
	m_mapAllPlayer_Online[player_id] = pConn;
	TRACEINFO("onPlayerLogin player_id:%ld session_id:%ld old_session_id:%ld", player_id, pConn->session_id, oldSession_id);

	return Proto::RESULT_SUCCESS;

}

void PlayerManager::onPlayerLogoff(INT64 player_id)
{
	SessionConn * pConn = getPlayerSession(player_id);
	INT64 session_id = 0;
	if (pConn != NULL) {
		pConn->player_id = 0;
		m_mapAllPlayer_Online.erase(player_id);
		session_id = pConn->session_id;
	}
	TRACEINFO("onPlayerLogoff player_id:%ld session_id:%ld", player_id, session_id);
}

bool PlayerManager::sendProtoToPlayer(INT64 player_id, INT32 messageID, INT32 errCode, ::google::protobuf::Message * proto)
{
	if (player_id <= 0 ) {
		TRACEERROR("sendProtoToPlayer failed,player_id :%ld", player_id);
		return false;
	}
	SessionConn * pConn = getPlayerSession(player_id);
	if (pConn == NULL) {
		return false;
	}
	
	return sendProtoToSession(pConn->session_id, messageID, errCode, proto);
}

bool PlayerManager::sendToPlayer(INT64 player_id, INT32 messageID, INT32 errCode, char * msg, INT32 msgLen){
	if (player_id <= 0 ) {
		TRACEERROR("sendProtoToPlayer failed,player_id :%ld", player_id);
		return false;
	}
	SessionConn * pConn = getPlayerSession(player_id);
	if (pConn == NULL) {
		return false;
	}
	
	return sendToSession(pConn->session_id, messageID, errCode, msg, msgLen);
}

bool PlayerManager::sendProtoToSession(INT64 session_id, INT32 messageID, INT32 errCode, ::google::protobuf::Message * proto)
{
	if (session_id <= 0  ) {
		TRACEERROR("sendProtoToSession failed,session_id :%ld", session_id);
		return false;
	}

	Packet* pPack = Packet::NewPacket(messageID, errCode);
	if (pPack == NULL) {
		TRACEERROR("sendProtoToSession failed,session_id :%ld,NewPacket failed", session_id);
		return false;
	}

	if (proto != NULL) {
		INT32 sendLen = proto->ByteSize();
		char buff[sendLen];

		if (sendLen <= 0 || proto->SerializeToArray(buff, sendLen)) {
			TRACEERROR("proto->SerializeToArray failed,session_id :%ld messageID:%d errCode:%d", session_id, messageID, errCode);
			ORYX_DEL(pPack);
			return false;
		}
		pPack->Append(buff, sendLen);

	}
	
	bool result = sendMsgToConn(session_id, pPack->data, pPack->message_len);
	ORYX_DEL(pPack);

	return result;
}

bool PlayerManager::sendToSession(INT64 player_id, INT32 messageID, INT32 errCode, char * msg, INT32 msgLen){
	if (session_id <= 0 || msgLen < 0 ) {
		TRACEERROR("sendToSession failed,session_id :%ld", session_id);
		return false;
	}

	Packet* pPack = Packet::NewPacket(messageID, errCode);
	if (pPack == NULL) {
		TRACEERROR("sendToSession failed,session_id :%ld,NewPacket failed", session_id);
		return false;
	}

	if (msg != NULL && msgLen > 0) {
		pPack->Append(msg, msgLen);
	}
	
	bool result = sendMsgToConn(session_id, pPack->data, pPack->message_len);
	ORYX_DEL(pPack);

	return result;
}
