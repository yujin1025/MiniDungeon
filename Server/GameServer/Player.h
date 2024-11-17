#pragma once
#include "Creature.h"

class GameSession;
class Room;

class Player : public Creature
{
public:
	Player();
	virtual ~Player();

public:
	GameSessionRef GetSession() const { return session.lock(); }

	const Protocol::PlayerInfo& GetPlayerInfo() { return *playerInfo; }
	void SetPlayerInfo(const Protocol::PlayerInfo& playerInfo) { this->playerInfo->CopyFrom(playerInfo); }
	void SetPlayerID(int64 playerID) { playerInfo->set_player_id(playerID); }

	const Protocol::ObjectInfo& GetObjectInfo() { return playerInfo->object_info(); }
	void SetObjectInfo(const Protocol::ObjectInfo& obj_Info);

	const Protocol::PosInfo& GetPosInfo() { return GetObjectInfo().pos_info(); }
	void SetPosInfo(const Protocol::PosInfo& pos_Info);

protected:
	Protocol::PlayerInfo* playerInfo;

public:
	atomic<weak_ptr<Lobby>> lobby;
	weak_ptr<GameSession> session;
};

