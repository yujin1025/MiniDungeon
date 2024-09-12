#pragma once
class Lobby : public JobQueue
{
public:
	Lobby();
	~Lobby();

public:
	unordered_map<uint64, RoomRef>& GetRooms() { return _rooms; }
	unordered_map<uint64, PlayerRef>& GetPlayers() { return _players; }

	bool AddRoom(RoomRef room);

	bool RemoveRoom(RoomRef room);

	bool AddPlayer(PlayerRef player);

	bool EnterLobby(PlayerRef player);

	bool HandleEnterPlayer(PlayerRef player);

	bool CreateRoom(const Protocol::RoomInfo& roomInfo);

	bool HandleCreateRoom(const Protocol::RoomInfo& roomInfo);

	bool JoinRoom(uint64 playerId, uint64 roomId);

	bool HandleJoinRoom(uint64 playerId, uint64 roomId);

private:
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

private:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, RoomRef> _rooms;

private:
	static atomic<int64> s_idGenerator;
};

extern LobbyRef GLobby;
