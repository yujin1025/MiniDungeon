#include "pch.h"
#include "Lobby.h"
#include "Player.h"
#include "Room.h"

LobbyRef GLobby = make_shared<Lobby>();
atomic<int64> Lobby::s_idGenerator = 1;

Lobby::Lobby()
{

}

Lobby::~Lobby()
{

}

bool Lobby::EnterLobby(PlayerRef player)
{
	Protocol::STC_ENTER_LOBBY enterLobbyPkt;

	if(_players.find(player->GetPlayerInfo()->player_id()) != _players.end())
	{
		enterLobbyPkt.set_success(false);
		return false;
	}

	_players.insert(make_pair(player->GetPlayerInfo()->player_id(), player));
	
	player->lobby.store(static_pointer_cast<Lobby>(shared_from_this()));
	{
		enterLobbyPkt.set_success(true);
		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
		playerInfo->CopyFrom(*player->GetPlayerInfo());

		enterLobbyPkt.set_allocated_player(playerInfo);
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterLobbyPkt);
	if (auto session = player->GetSession())
		session->Send(sendBuffer);

	return true;
}

bool Lobby::HandleEnterPlayer(PlayerRef player)
{
	return EnterLobby(player);
}

bool Lobby::CreateRoom(PlayerRef player)
{
	Protocol::STC_CREATE_ROOM createRoomPkt;

	// 로비에 없으면 문제있음
	if (_players.find(player->GetPlayerInfo()->player_id()) == _players.end())
	{
		createRoomPkt.set_success(false);
		return false;
	}

	const int64 newId = s_idGenerator.fetch_add(1);
	RoomRef room = make_shared<Room>();


	//_rooms.insert(make_pair(s_idGenerator, room));

	return false;
}

bool Lobby::CreateRoom(const Protocol::PlayerInfo& info)
{
	Protocol::STC_CREATE_ROOM createRoomPkt;

	// 로비에 없으면 문제있음
	if (_players.find(info.player_id()) == _players.end())
	{
		createRoomPkt.set_success(false);
		return false;
	}

	createRoomPkt.set_success(true);

	Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
	playerInfo->CopyFrom(info);
	createRoomPkt.set_allocated_host(playerInfo);

	const int64 newId = s_idGenerator.fetch_add(1);
	createRoomPkt.set_roomindex(newId);

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(createRoomPkt);
	if (auto session = _players[info.player_id()]->session.lock())
	{
		session->Send(sendBuffer);
	}

	RoomRef room = make_shared<Room>();
	_rooms.insert(make_pair(newId, room));

	return true;
}

//bool Lobby::HandleCreateRoom(PlayerRef player)
//{
//	return CreateRoom(player);
//}

bool Lobby::HandleCreateRoom(const Protocol::PlayerInfo& playerInfo)
{

	return CreateRoom(playerInfo);
}


