#pragma once
class Lobby : public JobQueue
{
public:
	Lobby();
	~Lobby();

public:
	unordered_map<uint64, RoomRef>& GetRooms() { return _rooms; }
	unordered_map<uint64, PlayerRef>& GetPlayers() { return _players; }

	bool EnterLobby(PlayerRef player);

	bool HandleEnterPlayer(PlayerRef player);

	bool CreateRoom(PlayerRef player);

	bool CreateRoom(const Protocol::PlayerInfo& playerInfo);

	//bool HandleCreateRoom(PlayerRef player);

	bool HandleCreateRoom(const Protocol::PlayerInfo& playerInfo);
private:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, RoomRef> _rooms;

private:
	static atomic<int64> s_idGenerator;
};

extern LobbyRef GLobby;
