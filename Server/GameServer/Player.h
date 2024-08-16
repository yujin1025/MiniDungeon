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
	GameSessionRef GetSession() { return session.lock(); }

	Protocol::PlayerInfo* GetPlayerInfo() { return playerInfo; }
	void SetPlayerInfo(Protocol::PlayerInfo* playerInfo) { this->playerInfo = playerInfo; }

protected:
	Protocol::PlayerInfo* playerInfo;

public:
	atomic<weak_ptr<Lobby>> lobby;
	weak_ptr<GameSession> session;
};

