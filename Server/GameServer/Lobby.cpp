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

bool Lobby::CreateRoom(const Protocol::PlayerInfo info, string roomName, string password)
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

	const uint64 newId = s_idGenerator.fetch_add(1);
	createRoomPkt.set_roomindex(newId);

	createRoomPkt.set_roomname(roomName);
	createRoomPkt.set_password(password);

	// 방 생성 사실을 생성한 클라이언트에게 전달
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(createRoomPkt);
	if (auto session = _players[playerInfo->player_id()]->session.lock())
	{
		session->Send(sendBuffer);
	}

	// 방 생성 사실을 다른 클라이언트들에게도 전달
	{
		Broadcast(sendBuffer, playerInfo->player_id());
	}

	RoomRef room = make_shared<Room>();
	room->SetRoomIndex(newId);

	_rooms.insert(make_pair(newId, room));

	return true;
}

//bool Lobby::HandleCreateRoom(PlayerRef player)
//{
//	return CreateRoom(player);
//}

bool Lobby::HandleCreateRoom(const Protocol::PlayerInfo info, string roomName, string password)
{
	return CreateRoom(info, roomName, password);
}

void Lobby::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& player : _players)
	{
		if (player.first == exceptId)
			continue;

		if (auto session = player.second->session.lock())
			session->Send(sendBuffer);
	}
}


