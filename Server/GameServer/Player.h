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
	void SetSession(GameSessionRef session) { this->session = session; }

	Protocol::PlayerInfo* GetPlayerInfo() { return playerInfo; }
	void SetPlayerInfo(Protocol::PlayerInfo* playerInfo) { this->playerInfo = playerInfo; }

protected:
	Protocol::PlayerInfo* playerInfo;
	weak_ptr<GameSession> session;

public:
	atomic<weak_ptr<Lobby>> lobby;
};

