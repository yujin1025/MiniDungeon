#pragma once
#include "JobQueue.h"

struct Vector3
{
	float x, y, z;
};


class Room : public JobQueue
{
public:
	Room();
	virtual ~Room();

public:
	virtual bool Enter(PlayerRef player);
	virtual bool Leave(PlayerRef player);
	//bool EnterRoom(ObjectRef object);

	bool EnterRoom(PlayerRef player, bool isHost = true);
	bool LeaveRoom(PlayerRef player);
	bool ChangeCharacter(uint64 playerIndex, const Protocol::PlayerType characterType);

	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(uint64 playerIndex);
	void HandleStartGame();

	bool HandleChangeCharacter(uint64 playerIndex, const Protocol::PlayerType characterType);
	void HandleMove(Protocol::CTS_MOVE pkt);

	uint64 GetRoomIndex() const { return _roomIndex; }
	void SetRoomIndex(uint64 roomIndex);

	void ReleaseThisRoom();
public:
	void UpdateTick();

	RoomRef GetRoomRef();

private:
	bool AddObject(ObjectRef object);
	bool RemoveObject(uint64 objectId);

	bool AddPlayer(PlayerRef player);
	bool RemovePlayer(PlayerRef player);
private:
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

	void BroadcastToPlayer(SendBufferRef sendBuffer, uint64 exceptId = 0);

private:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, ObjectRef> _objects;
	weak_ptr<class Lobby> _lobby;
	uint64 _roomIndex = 0;

private:
	unordered_map<uint64, Vector3> _spawnPoints;

protected:
	Protocol::RoomInfo* info;

public:
	const Protocol::RoomInfo* GetRoomInfo() { return info; }

	void SetRoomInfo(const Protocol::RoomInfo& info) { this->info->CopyFrom(info); }

	void SetLobby(LobbyRef lobby) { this->_lobby = lobby; }
};

extern RoomRef GRoom;