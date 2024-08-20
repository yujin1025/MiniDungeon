#pragma once
#include "JobQueue.h"


class Room : public JobQueue
{
public:
	Room();
	virtual ~Room();

public:
	bool EnterRoom(ObjectRef object, bool isHost = true);
	bool LeaveRoom(ObjectRef object);

	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	void HandleMove(Protocol::CTS_MOVE pkt);

	uint64 GetRoomIndex() const { return _roomIndex; }
	void SetRoomIndex(uint64 roomIndex) { _roomIndex = roomIndex; }
public:
	void UpdateTick();

	RoomRef GetRoomRef();

private:
	bool AddObject(ObjectRef object);
	bool RemoveObject(uint64 objectId);

private:
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

private:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, ObjectRef> _objects;
	weak_ptr<class Lobby> _lobby;
	uint64 _roomIndex;
};

extern RoomRef GRoom;