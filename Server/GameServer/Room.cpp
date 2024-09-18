#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"
#include "Monster.h"
#include "ObjectUtils.h"
#include "Lobby.h"

RoomRef GRoom = make_shared<Room>();

Room::Room()
{
	_roomIndex = 0;
	info = new Protocol::RoomInfo();

	for (int i = 0; i < 4; i++)
	{
		Vector3 pos = { 940 + 50 * i, 400 + 50 * i, 120 };

		_spawnPoints.insert(make_pair(i, pos));
	}
}

Room::~Room()
{
	_players.clear();
	_objects.clear();
	_spawnPoints.clear();

	info->Clear();
	info = nullptr;
}

bool Room::Enter(PlayerRef player)
{
	return false;
}

bool Room::Leave(PlayerRef player)
{
	return false;
}

//bool Room::LeaveRoom(ObjectRef object)
//{
//	if (object == nullptr)
//		return false;
//
//	//const uint64 objectId = object->objectInfo->object_id();
//	//bool success = RemoveObject(objectId);
//
//	// 퇴장 사실을 퇴장하는 플레이어에게 알린다
//	if (auto player = dynamic_pointer_cast<Player>(object))
//	{
//		Protocol::STC_LEAVE_GAME leaveGamePkt;
//
//		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(leaveGamePkt);
//		/*if (auto session = player->session.lock())
//			session->Send(sendBuffer);*/
//	}
//
//	// 퇴장 사실을 알린다
//	{
//		Protocol::STC_DESPAWN despawnPkt;
//		//despawnPkt.add_object_ids(objectId);
//
//		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(despawnPkt);
//		//Broadcast(sendBuffer, objectId);
//
//		//if (auto player = dynamic_pointer_cast<Player>(object))
//		//	if (auto session = player->session.lock())
//		//		session->Send(sendBuffer);
//	}
//
//	//return success;
//
//	return true;
//}

bool Room::EnterRoom(PlayerRef player, bool isHost)
{
	bool success = AddPlayer(player);

	if (!isHost)
	{
		// 방에 입장한 사실을 방에 있는 다른 플레이어들에게 알린다
		{
			Protocol::STC_JOIN_ROOM joinRoomPkt;
			joinRoomPkt.set_success(success);

			Protocol::RoomInfo* roomInfo = new Protocol::RoomInfo();
			roomInfo->CopyFrom(*info);
			joinRoomPkt.set_allocated_room_info(roomInfo);

			Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
			playerInfo->CopyFrom(*player->GetPlayerInfo());
			joinRoomPkt.set_allocated_player(playerInfo);

			SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(joinRoomPkt);
			BroadcastToPlayer(sendBuffer, player->GetObjectInfo()->object_id());
		}
	}

	return success;
}

bool Room::LeaveRoom(PlayerRef player)
{
	bool success = RemovePlayer(player);

	Protocol::STC_LEAVE_ROOM leaveRoomPkt;
	leaveRoomPkt.set_success(success);

	Protocol::RoomInfo* roomInfo = new Protocol::RoomInfo();
	roomInfo->CopyFrom(*info);
	leaveRoomPkt.set_allocated_room_info(roomInfo);

	leaveRoomPkt.set_player_id(player->GetPlayerInfo()->player_id());

	for (const auto& room : _lobby.lock()->GetRooms())
	{
		if (room.second->_players.empty())
		{
			continue;
		}

		leaveRoomPkt.add_rooms()->CopyFrom(*(room.second->GetRoomInfo()));
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(leaveRoomPkt);

	// 퇴장 사실을 퇴장하는 플레이어에게 알린다
	player->GetSession()->Send(sendBuffer);

	// 퇴장 사실을 Room에 있는 모든 플레이어에게 알린다.
	BroadcastToPlayer(sendBuffer, player->GetObjectInfo()->object_id());

	if (_players.empty())
	{
		ReleaseThisRoom();
	}

	return success;
}

bool Room::ChangeCharacter(uint64 playerIndex, const Protocol::PlayerType characterType)
{
	Protocol::STC_CHANGE_CHARACTER changeCharacterPkt;

	// 플레이어가 방에 없다면 문제가 있다.
	if (_players.find(playerIndex) == _players.end())
	{
		return false;
	}

	// 캐릭터 변경 사실을 알린다.
	changeCharacterPkt.set_success(true);
	changeCharacterPkt.set_player_id(playerIndex);
	changeCharacterPkt.set_character(characterType);
	changeCharacterPkt.set_roomindex(_roomIndex);

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(changeCharacterPkt);
	BroadcastToPlayer(sendBuffer, _players[playerIndex]->GetObjectInfo()->object_id());

	return true;
}

bool Room::HandleChangeCharacter(uint64 playerIndex, const Protocol::PlayerType characterType)
{
	return ChangeCharacter(playerIndex, characterType);
}

bool Room::HandleEnterPlayer(PlayerRef player)
{
	return EnterRoom(player, true);
}

bool Room::HandleLeavePlayer(uint64 playerindex)
{
	auto& leavedPlayer = _players[playerindex];
	
	if (leavedPlayer != nullptr)
	{
		return LeaveRoom(leavedPlayer);
	}

	return true;
}

//bool Room::EnterRoom(ObjectRef object)
//{
//	bool success = AddObject(object);
//
//	// 랜덤 위치
//
//	// 입장 사실을 신입 플레이어에게 알린다
//	if (auto player = dynamic_pointer_cast<Player>(object))
//	{
//		//Protocol::STC_ENTER_GAME enterGamePkt;
//		//enterGamePkt.set_success(success);
//
//		Protocol::ObjectInfo* playerInfo = new Protocol::ObjectInfo();
//		//playerInfo->CopyFrom(*object->objectInfo);
//		//enterGamePkt.set_allocated_player(playerInfo);
//
//		///SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
//		//if (auto session = player->session.lock())
//		//	session->Send(sendBuffer);
//	}
//
//	// 입장 사실을 다른 플레이어에게 알린다
//	{
//		Protocol::STC_SPAWN spawnPkt;
//
//		Protocol::ObjectInfo* objectInfo = spawnPkt.add_players();
//		//objectInfo->CopyFrom(*object->objectInfo);
//
//		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
//		//Broadcast(sendBuffer, object->objectInfo->object_id());
//	}
//
//	// 기존 입장한 플레이어 목록을 신입 플레이어한테 전송해준다
//	if (auto player = dynamic_pointer_cast<Player>(object))
//	{
//		Protocol::STC_SPAWN spawnPkt;
//
//		for (auto& item : _objects)
//		{
//			if (item.second->IsPlayer() == false)
//				continue;
//
//			Protocol::ObjectInfo* playerInfo = spawnPkt.add_players();
//			//playerInfo->CopyFrom(*item.second->objectInfo);
//		}
//
//		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
//		//if (auto session = player->session.lock())
//		//	session->Send(sendBuffer);
//	}
//
//	return success;
//}

void Room::HandleStartGame()
{
	Protocol::STC_ENTER_GAME enterGamePkt;
	enterGamePkt.set_success(true);

	int index = 0;
	for(auto& player : _players)
	{
		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
		Protocol::ObjectInfo* objectInfo = new Protocol::ObjectInfo();
		Protocol::PosInfo* posInfo = new Protocol::PosInfo();

		playerInfo->CopyFrom(*player.second->GetPlayerInfo());
		objectInfo->CopyFrom(*player.second->GetObjectInfo());
		posInfo->CopyFrom(*player.second->GetPosInfo());

		posInfo->set_x(_spawnPoints[index].x);
		posInfo->set_y(_spawnPoints[index].y);
		posInfo->set_z(_spawnPoints[index].z);
		index++;

		objectInfo->set_allocated_pos_info(posInfo);
		playerInfo->set_allocated_object_info(objectInfo);

		enterGamePkt.add_players()->CopyFrom(*playerInfo);
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	Broadcast(sendBuffer);
}

void Room::HandleMove(Protocol::CTS_MOVE pkt)
{
	const uint64 objectId = pkt.info().object_id();
	if (_objects.find(objectId) == _objects.end())
		return;

	// 적용
	PlayerRef player = dynamic_pointer_cast<Player>(_objects[objectId]);
	//player->posInfo->CopyFrom(pkt.info());

	// 이동 사실을 알린다 (본인 포함? 빼고?)
	{
		Protocol::STC_MOVE movePkt;
		{
			Protocol::PosInfo* info = movePkt.mutable_info();
			info->CopyFrom(pkt.info());
		}
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
		Broadcast(sendBuffer);
	}
}

void Room::SetRoomIndex(uint64 roomIndex)
{
	_roomIndex = roomIndex;
	info->set_room_id(roomIndex);	
}

void Room::UpdateTick()
{
	cout << "Update Room" << endl;

	//DoTimer(100, &Room::UpdateTick);
}

RoomRef Room::GetRoomRef()
{
	return static_pointer_cast<Room>(shared_from_this());
}

void Room::ReleaseThisRoom()
{
	auto self = GetRoomRef();
	_lobby.lock()->RemoveRoom(self);
}

bool Room::AddObject(ObjectRef object)
{
	// 있다면 문제가 있다.
	if (_objects.find(object->GetObjectInfo()->object_id()) != _objects.end())
	{
		return false;
	}

	_objects.insert(make_pair(object->GetObjectInfo()->object_id(), object));

	object->room.store(GetRoomRef());

	return true;
}

bool Room::RemoveObject(uint64 objectId)
{
	// Room에 없다면 문제가 있다.
	if (_objects.find(objectId) == _objects.end())
		return false;

	ObjectRef object = _objects[objectId];
	PlayerRef player = dynamic_pointer_cast<Player>(object);
	if (player)
		player->room.store(weak_ptr<Room>());

	_objects.erase(objectId);

	return true;
}

bool Room::AddPlayer(PlayerRef player)
{
	// Room에 있다면 문제가 있다.
	if (_players.find(player->GetPlayerInfo()->player_id()) != _players.end())
	{
		return false;
	}
	player->SetObjectID(ObjectUtils::GetNewObjectID());

	_players.insert(make_pair(player->GetPlayerInfo()->player_id(), player));
	_objects.insert(make_pair(player->GetObjectInfo()->object_id(), player));
	player->room.store(GetRoomRef());


	info->set_current_player_count(_players.size());
	info->add_players()->CopyFrom(*player->GetPlayerInfo());

	return true;
}

bool Room::RemovePlayer(PlayerRef player)
{
	uint64 playerIndex = player->GetPlayerInfo()->player_id();
	// 플레이어가 Room에 없으면 문제가 있다.
	if (_players.find(playerIndex) == _players.end())
	{
		return false;
	}
	
	_lobby.lock()->AddPlayer(player);
	info->clear_players();

	if (info->host().player_id() == playerIndex)
	{
		info->clear_host();
		auto originHost = _players.find(playerIndex);
		
		if (next(originHost) != _players.end())
		{ 
			Protocol::PlayerInfo* newHost = new Protocol::PlayerInfo();
			newHost->CopyFrom(*(next(originHost)->second->GetPlayerInfo()));
			info->set_allocated_host(newHost);
		}
	}

	_players.erase(playerIndex);
	info->set_current_player_count(_players.size());

	for (const auto& player : _players)
	{
		info->add_players()->CopyFrom(*(player.second->GetPlayerInfo()));
	}

	return true;
}

void Room::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& item : _objects)
	{
		PlayerRef player = dynamic_pointer_cast<Player>(item.second);
		if (player == nullptr)
			continue;

		if (player->GetObjectInfo()->object_id() == exceptId)
			continue;

		if (GameSessionRef session = player->session.lock())
			session->Send(sendBuffer);
	}
}

void Room::BroadcastToPlayer(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& player : _players)
	{
		if (player.second->GetObjectInfo()->object_id() == exceptId)
			continue;

		if (GameSessionRef session = player.second->GetSession())
			session->Send(sendBuffer);
	}
}
