#pragma once
class Lobby : public enable_shared_from_this<Lobby>
{
public:
	Lobby();
	~Lobby();

public:
	unordered_map<uint64, RoomRef>& GetRooms() { return _rooms; }

	bool EnterLobby(PlayerRef player);
private:
	unordered_map<string, PlayerRef> _players;
	unordered_map<uint64, RoomRef> _rooms;
	JobQueueRef _jobQueue = nullptr;
};

extern LobbyRef GLobby;
