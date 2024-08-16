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
private:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, RoomRef> _rooms;
};

extern LobbyRef GLobby;
