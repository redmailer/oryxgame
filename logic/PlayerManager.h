#ifndef __PLAYER_MANAGER__
#define __PLAYER_MANAGER__

#include "../oryx/common.h"
#include <unordered_map>
#include "Player.h"
#include "../oryx/ClientConnManager.h"
#include <google/protobuf/message.h>

typedef std::unordered_map<INT64, Player *> PlayerMap;
typedef std::unordered_map<INT64, SessionConn *> PlayerSessionMap;	//playeID : sessionID

class PlayerManager :public ClientConnManager {
public:
	PlayerManager();

	virtual bool init();

	virtual void onSessionClose(SessionConn * pConn);
	INT32 onPlayerLogin(INT64 player_id, SessionConn * pConn);
	void onPlayerLogoff(INT64 player_id);

	bool sendProtoToPlayer(INT64 player_id, INT32 messageID, INT32 errCode, ::google::protobuf::Message * proto = NULL);
	bool sendProtoToSession(INT64 session_id, INT32 messageID, INT32 errCode, ::google::protobuf::Message * proto = NULL);


public:

	inline Player * getPlayer(INT64 player_id) {
		PlayerMap::iterator it = m_mapPlayer_All.find(player_id);
		if (it != m_mapPlayer_All.end()) {
			return it->second;
		}
		return NULL;
	}

	inline bool isPlayerOnline(INT64 player_id) {
		PlayerSessionMap::iterator it = m_mapAllPlayer_Online.find(player_id);
		if (it != m_mapAllPlayer_Online.end()) {
			return true;
		}
		return false;
	}

	inline SessionConn * getPlayerSession(INT64 player_id) {
		PlayerSessionMap::iterator it = m_mapAllPlayer_Online.find(player_id);
		if (it != m_mapAllPlayer_Online.end()) {

			if (it->second == NULL) {
				m_mapAllPlayer_Online.erase(it);
				return NULL;
			}
			return it->second;

		}
		return NULL;
	};


	
	


private:
	PlayerMap m_mapPlayer_All;			//所有玩家的缓存列表

	PlayerMap m_mapAllPlayer_Dirty;		//需要保存的玩家

	PlayerSessionMap m_mapAllPlayer_Online;	//在线玩家的列表

	INT64 m_iAutoIncrPlayerID;

};


#endif
