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

	//不能重复登录
	if (pConn->player_id == player_id) {
		TRACEWARN("onPlayerLogin repeated player:%ld", player_id);
		return Proto::RESULT_LOGIN_REPEATED;
	}

	Player * pPlayer = getPlayer(player_id);
	if (pPlayer == NULL){
		TRACEERROR("onPlayerLogin failed cannot find player:%ld ", player_id);
		//玩家不存在，记得返回错误
		return Proto::RESULT_LOGIN_NOEXIST;
	}

	//异地登录 处理
	INT64 oldSession_id = 0;
	SessionConn * pOldConn = getPlayerSession(player_id);
	if (pOldConn != NULL) {
		//这里不判断pOldConn 的 player_id。如果是错的，也应该让他下线
		pOldConn->player_id = 0;
		oldSession_id = pOldConn->session_id;
	}
	pConn->player_id = player_id;
	m_mapAllPlayer_Online[player_id] = pConn;
	TRACEINFO("onPlayerLogin player_id:%ld session_id:%ld old_session_id:%ld", player_id, pConn->session_id, oldSession_id);
	//处理玩家登录的数据并返回

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
	
	//处理玩家下线
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
	
	bool Result = sendMsgToConn(session_id, pPack->data, pPack->message_len);
	ORYX_DEL(pPack);

	return Result;
}
