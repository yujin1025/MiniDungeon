#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"
#include "Monster.h"
#include "ObjectUtils.h"

RoomRef GRoom = make_shared<Room>();

Room::Room()
{

	info = new Protocol::RoomInfo();
}

Room::~Room()
{
	_players.clear();
	_objects.clear();
	info->Clear();
}

bool Room::EnterRoom(ObjectRef object)
{
	bool success = AddObject(object);

	// 랜덤 위치

	// 입장 사실을 신입 플레이어에게 알린다
	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		//Protocol::STC_ENTER_GAME enterGamePkt;
		//enterGamePkt.set_success(success);

		Protocol::ObjectInfo* playerInfo = new Protocol::ObjectInfo();
		//playerInfo->CopyFrom(*object->objectInfo);
		//enterGamePkt.set_allocated_player(playerInfo);

		///SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
		//if (auto session = player->session.lock())
		//	session->Send(sendBuffer);
	}

	// 입장 사실을 다른 플레이어에게 알린다
	{
		Protocol::STC_SPAWN spawnPkt;

		Protocol::ObjectInfo* objectInfo = spawnPkt.add_players();
		//objectInfo->CopyFrom(*object->objectInfo);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		//Broadcast(sendBuffer, object->objectInfo->object_id());
	}

	// 기존 입장한 플레이어 목록을 신입 플레이어한테 전송해준다
	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::STC_SPAWN spawnPkt;

		for (auto& item : _objects)
		{
			if (item.second->IsPlayer() == false)
				continue;

			Protocol::ObjectInfo* playerInfo = spawnPkt.add_players();
			//playerInfo->CopyFrom(*item.second->objectInfo);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		//if (auto session = player->session.lock())
		//	session->Send(sendBuffer);
	}

	return success;
}

bool Room::LeaveRoom(ObjectRef object)
{
	if (object == nullptr)
		return false;

	//const uint64 objectId = object->objectInfo->object_id();
	//bool success = RemoveObject(objectId);

	// 퇴장 사실을 퇴장하는 플레이어에게 알린다
	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::STC_LEAVE_GAME leaveGamePkt;

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(leaveGamePkt);
		/*if (auto session = player->session.lock())
			session->Send(sendBuffer);*/
	}

	// 퇴장 사실을 알린다
	{
		Protocol::STC_DESPAWN despawnPkt;
		//despawnPkt.add_object_ids(objectId);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(despawnPkt);
		//Broadcast(sendBuffer, objectId);

		//if (auto player = dynamic_pointer_cast<Player>(object))
		//	if (auto session = player->session.lock())
		//		session->Send(sendBuffer);
	}

	//return success;

	return true;
}

bool Room::EnterRoom(PlayerRef player, bool isHost)
{
	bool success = AddPlayer(player);
	player->room.store(GetRoomRef());

	if (!isHost)
	{

	}

	return success;
}

bool Room::LeaveRoom(PlayerRef player)
{
	return false;
}

bool Room::HandleEnterPlayer(PlayerRef player)
{
	return EnterRoom(player, true);
}

bool Room::HandleLeavePlayer(PlayerRef player)
{
	return LeaveRoom(player);
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

bool Room::AddObject(ObjectRef object)
{
	// 있다면 문제가 있다.
	/*if (_objects.find(object->objectInfo->object_id()) != _objects.end())
		return false;

	_objects.insert(make_pair(object->objectInfo->object_id(), object));*/

	object->room.store(GetRoomRef());

	return true;
}

bool Room::RemoveObject(uint64 objectId)
{
	// 없다면 문제가 있다.
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
	if (_players.find(player->GetPlayerInfo()->player_id()) != _players.end())
	{
		return false;
	}

	_players.insert(make_pair(player->GetPlayerInfo()->player_id(), player));

	info->set_current_player_count(_players.size());
	info->add_players()->CopyFrom(*player->GetPlayerInfo());

	return true;
}

bool Room::RemovePlayer(uint64 playerId)
{
	return false;
}

void Room::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& item : _objects)
	{
		PlayerRef player = dynamic_pointer_cast<Player>(item.second);
		if (player == nullptr)
			continue;
		//if (player->objectInfo->object_id() == exceptId)
		//	continue;

		//if (GameSessionRef session = player->session.lock())
		//	session->Send(sendBuffer);
	}
}