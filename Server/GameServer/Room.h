#pragma once
#include "JobQueue.h"


class Room : public JobQueue
{
public:
	Room();
	virtual ~Room();

public:
	bool EnterRoom(ObjectRef object);
	bool LeaveRoom(ObjectRef object);

	bool EnterRoom(PlayerRef player, bool isHost = true);
	bool LeaveRoom(PlayerRef player);

	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	void HandleMove(Protocol::CTS_MOVE pkt);

	uint64 GetRoomIndex() const { return _roomIndex; }
	void SetRoomIndex(uint64 roomIndex);
public:
	void UpdateTick();

	RoomRef GetRoomRef();

private:
	bool AddObject(ObjectRef object);
	bool RemoveObject(uint64 objectId);

	bool AddPlayer(PlayerRef player);
	bool RemovePlayer(uint64 playerId);
private:
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

	void BroadcastToPlayer(SendBufferRef sendBuffer, uint64 exceptId = 0);

private:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, ObjectRef> _objects;
	weak_ptr<class Lobby> _lobby;
	uint64 _roomIndex;

protected:
	Protocol::RoomInfo* info;

public:
	const Protocol::RoomInfo* GetRoomInfo() { return info; }

	void SetRoomInfo(const Protocol::RoomInfo& info) { this->info->CopyFrom(info); }
};

extern RoomRef GRoom;