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

	const Protocol::PlayerInfo& GetPlayerInfo() const { READ_LOCK; return *playerInfo; }
	void SetPlayerInfo(const Protocol::PlayerInfo& playerInfo) { WRITE_LOCK; this->playerInfo->CopyFrom(playerInfo); }
	void SetPlayerID(int64 playerID) { WRITE_LOCK; playerInfo->set_player_id(playerID); }
	void SetPlayerType(Protocol::PlayerType playerType) { WRITE_LOCK; playerInfo->set_player_type(playerType); }

	const Protocol::ObjectInfo& GetObjectInfo() const { READ_LOCK; return playerInfo->object_info(); }
	void SetObjectInfo(const Protocol::ObjectInfo& obj_Info);

	const Protocol::PosInfo& GetPosInfo() const { READ_LOCK; return objectInfo->pos_info(); }
	void SetPosInfo(const Protocol::PosInfo& pos_Info);

	void Attack(const Protocol::AttackInfo& attack_info);
	void ProcessAttack(int32 skillType, float damage, float maxDistance, float maxAngle);
protected:
	Protocol::PlayerInfo* playerInfo;

public:
	atomic<weak_ptr<Lobby>> lobby;
	weak_ptr<GameSession> session;

private:
	mutable USE_LOCK;
};

