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

		for (auto& roomInfo : _rooms)
		{
			Protocol::RoomInfo* info = enterLobbyPkt.add_rooms();
			info->CopyFrom(*(roomInfo.second->GetRoomInfo()));
		}
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

bool Lobby::CreateRoom(const Protocol::RoomInfo& roomInfo)
{
	Protocol::STC_CREATE_ROOM createRoomPkt;

	// 로비에 없으면 문제있음
	if (_players.find(roomInfo.host().player_id()) == _players.end())
	{
		createRoomPkt.set_success(false);
		return false;
	}

	// 방 생성
	RoomRef room = make_shared<Room>();
	room->SetRoomInfo(roomInfo);

	const uint64 newId = s_idGenerator.fetch_add(1);
	room->SetRoomIndex(newId);

	uint64 host_id = room->GetRoomInfo()->host().player_id();

	room->EnterRoom(_players[host_id], true);

	_rooms.insert(make_pair(newId, room));

	// 방 생성 성공 시 패킷 세팅
	createRoomPkt.set_success(true);

	Protocol::RoomInfo* info = new Protocol::RoomInfo();

	info->CopyFrom(*room->GetRoomInfo());

	createRoomPkt.set_allocated_room_info(info);

	// 방 생성 사실을 생성한 클라이언트에게 전달
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(createRoomPkt);
	if (auto session = _players[host_id]->session.lock())
	{
		session->Send(sendBuffer);
	}

	// 방 생성 사실을 다른 클라이언트들에게도 전달
	{
		Broadcast(sendBuffer, host_id);
	}

	_players.erase(host_id);
	return true;
}

bool Lobby::HandleCreateRoom(const Protocol::RoomInfo& roomInfo)
{
	return CreateRoom(roomInfo);
}

bool Lobby::JoinRoom(uint64 playerId, uint64 roomId)
{
	Protocol::STC_JOIN_ROOM joinRoomPkt;

	// 플레이어가 로비에 없으면 문제있음
	if(_players.find(playerId) == _players.end())
	{
		joinRoomPkt.set_success(false);
		return false;
	}

	// 방이 없으면 문제있음
	if(_rooms.find(roomId) == _rooms.end())
	{
		joinRoomPkt.set_success(false);
		return false;
	}

	// 방에 입장
	if (_rooms[roomId]->EnterRoom(_players[playerId], false))
	{
		// 방 입장 성공 시 패킷 세팅
		joinRoomPkt.set_success(true);

		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
		playerInfo->CopyFrom(*_players[playerId]->GetPlayerInfo());
		joinRoomPkt.set_allocated_player(playerInfo);

		Protocol::RoomInfo* roomInfo = new Protocol::RoomInfo();
		roomInfo->CopyFrom(*_rooms[roomId]->GetRoomInfo());
		joinRoomPkt.set_allocated_room_info(roomInfo);
	}
	else
	{
		// 방 입장 실패 시 패킷 세팅
		joinRoomPkt.set_success(false);
	}

	// 방 입장 사실을 클라이언트에게 전달
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(joinRoomPkt);
	if (auto session = _players[playerId]->session.lock())
	{
		session->Send(sendBuffer);
	}

	_players.erase(playerId);
	return true;
}

bool Lobby::HandleJoinRoom(const Protocol::CTS_JOIN_ROOM& pkt)
{
	uint64 playerId = pkt.player().player_id();
	uint64 roomId = pkt.roomindex();

	return JoinRoom(playerId, roomId);
}

bool Lobby::HandleJoinRoom(uint64 playerId, uint64 roomId)
{
	return JoinRoom(playerId, roomId);
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


