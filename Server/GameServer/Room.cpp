#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"
#include "Monster.h"
#include "ObjectUtils.h"
#include "Lobby.h"
#include "GameSessionManager.h"

Room::Room()
{
	_roomIndex = 0;
	info = new Protocol::RoomInfo();
}

Room::~Room()
{
	_players.clear();
	_objects.clear();
	ClearJobs();

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
		Protocol::STC_JOIN_ROOM joinRoomPkt;
		joinRoomPkt.set_success(success);

		Protocol::RoomInfo* roomInfo = new Protocol::RoomInfo();
		roomInfo->CopyFrom(*info);
		joinRoomPkt.set_allocated_room_info(roomInfo);

		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
		playerInfo->CopyFrom(player->GetPlayerInfo());
		joinRoomPkt.set_allocated_player(playerInfo);

		// �濡 ������ ����� �濡 �ִ� �ٸ� �÷��̾�鿡�� �˸���
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(joinRoomPkt);
		Broadcast(sendBuffer, player->GetObjectInfo().object_id());
	}

	return success;
}

bool Room::LeaveRoom(PlayerRef player, bool isExitGame)
{
	bool success = RemovePlayer(player, isExitGame);

	Protocol::STC_LEAVE_ROOM leaveRoomPkt;
	leaveRoomPkt.set_success(success);

	Protocol::RoomInfo* roomInfo = new Protocol::RoomInfo();
	roomInfo->CopyFrom(*info);
	leaveRoomPkt.set_allocated_room_info(roomInfo);

	leaveRoomPkt.set_player_id(player->GetPlayerInfo().player_id());

	for (const auto& room : _lobby.lock()->GetRooms())
	{
		if (room.second->_players.empty())
		{
			continue;
		}

		leaveRoomPkt.add_rooms()->CopyFrom(*(room.second->GetRoomInfo()));
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(leaveRoomPkt);

	if (isExitGame == false)
	{
		// 퇴장 사실을 퇴장하는 플레이어에게 알린다
		player->GetSession()->Send(sendBuffer);
	}

	// ���� ����� Room�� �ִ� ��� �÷��̾�� �˸���.
	Broadcast(sendBuffer, player->GetObjectInfo().object_id());

	// ���� ����� Lobby�� �ִ� ��� �÷��̾�Ե� �˸���.
	_lobby.lock()->Broadcast(sendBuffer, player->GetPlayerInfo().player_id());

	if (_players.empty())
	{
		ReleaseThisRoom();
	}

	GSessionManager.Remove(player->GetSession());

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

	_players[playerIndex]->SetPlayerType(characterType);

	// 캐릭터 변경 사실을 알린다.
	changeCharacterPkt.set_success(true);
	changeCharacterPkt.set_player_id(playerIndex);
	changeCharacterPkt.set_character(characterType);
	changeCharacterPkt.set_roomindex(_roomIndex);

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(changeCharacterPkt);
	Broadcast(sendBuffer, _players[playerIndex]->GetObjectInfo().object_id());

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

bool Room::HandleLeavePlayer(uint64 playerindex, bool isExitGame)
{
	auto& leavedPlayer = _players[playerindex];
	
	if (leavedPlayer != nullptr)
	{
		return LeaveRoom(leavedPlayer, isExitGame);
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

		playerInfo->CopyFrom(player.second->GetPlayerInfo());
		objectInfo->CopyFrom(player.second->GetObjectInfo());
		posInfo->CopyFrom(player.second->GetPosInfo());

		objectInfo->set_allocated_pos_info(posInfo);
		playerInfo->set_allocated_object_info(objectInfo);

		enterGamePkt.add_players()->CopyFrom(*playerInfo);
	}

	SpawnMonsters();

	for (auto& monster : _monsters)
	{
		Protocol::MonsterInfo* monsterInfo = new Protocol::MonsterInfo();
		Protocol::ObjectInfo* objectInfo = new Protocol::ObjectInfo();
		Protocol::PosInfo* posInfo = new Protocol::PosInfo();

		monsterInfo->CopyFrom(monster.second->GetMonsterInfo());
		objectInfo->CopyFrom(monster.second->GetObjectInfo());
		posInfo->CopyFrom(monster.second->GetPosInfo());

		enterGamePkt.add_monsters()->CopyFrom(*monsterInfo);
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	Broadcast(sendBuffer);

	GetRoomRef()->DoAsync(&Room::UpdateTick);
}

//void Room::HandleMove(Protocol::CTS_MOVE pkt)
//{
//	const uint64 objectId = pkt.info().object_id();
//	if (_objects.find(objectId) == _objects.end())
//		return;
//
//	// 적용
//	PlayerRef player = dynamic_pointer_cast<Player>(_objects[objectId]);
//	if (!player)
//		return;
//
//	// 최신 위치 정보로 업데이트
//	player->posInfo->CopyFrom(pkt.info());
//
//	// 이동 사실을 알린다 (본인 포함? 빼고?)
//	Protocol::STC_MOVE movePkt;
//	Protocol::PosInfo* info = new Protocol::PosInfo();
//	info->CopyFrom(*player->posInfo);
//	movePkt.set_allocated_info(info);
//
//	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
//	BroadcastToPlayer(sendBuffer, objectId);
//	//Broadcast(sendBuffer);
//}

void Room::HandleMove(const Protocol::PosInfo& posInfo)
{
	const uint64 objectId = posInfo.object_id();
	if (_objects.find(objectId) == _objects.end())
		return;

	// 적용
	PlayerRef player = dynamic_pointer_cast<Player>(_objects[objectId]);
	if (!player)
	{
		HandleMoveMonster(posInfo);
	}
	else
	{
		if(player->GetHp() <= 0)
		{
			HandleDead(objectId);
			return;
		}
		// 최신 위치 정보로 업데이트
		player->SetPosInfo(posInfo);

		// 이동 사실을 알린다 (본인 포함? 빼고?)
		Protocol::STC_MOVE movePkt;
		Protocol::PosInfo* info = new Protocol::PosInfo();
		info->CopyFrom(player->GetPosInfo());
		movePkt.set_allocated_info(info);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
		Broadcast(sendBuffer, objectId);
	}
}

void Room::HandleMoveMonster(const Protocol::PosInfo& info)
{
	const uint64 objectId = info.object_id();
	if (_objects.find(objectId) == _objects.end())
		return;

	// 적용
	MonsterRef monster = dynamic_pointer_cast<Monster>(_objects[objectId]);
	if (!monster)
		return;

	// 최신 위치 정보로 업데이트
	monster->SetPosInfo(info);

	//// 이동 사실을 알린다
	//Protocol::STC_MOVE movePkt;
	//Protocol::PosInfo* posInfo = new Protocol::PosInfo();
	//posInfo->CopyFrom(info);
	//movePkt.set_allocated_info(posInfo);

	//SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
	//Broadcast(sendBuffer);
}

void Room::HandleAttack(Protocol::CTS_ATTACK pkt)
{
	const uint64 objectId = pkt.info().object_id();
	if (_objects.find(objectId) == _objects.end())
		return;

	Protocol::STC_ATTACK atkPkt;
	Protocol::AttackInfo* info = new Protocol::AttackInfo();
	info->CopyFrom(pkt.info());
	atkPkt.set_allocated_info(info);

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(atkPkt);
	Broadcast(sendBuffer);
}

void Room::HandleAttacked(const Protocol::CTS_ATTACKED& pkt)
{
	const uint64 objectId = pkt.object_id();
	if (_objects.find(objectId) == _objects.end())
		return;

	if(pkt.object_current_hp() <= 0)
	{
		Protocol::STC_DESPAWN despawnPkt;
		despawnPkt.add_object_ids(objectId);
		Broadcast(ServerPacketHandler::MakeSendBuffer(despawnPkt));

		HandleDead(objectId);
	}
	else
	{
		dynamic_pointer_cast<Creature>(_objects[objectId])->SetHp(pkt.object_current_hp());
	}
}

void Room::HandleDead(uint64 object_id)
{
	const uint64 objectId = object_id;
	if (_objects.find(objectId) == _objects.end())
		return;

	if (dynamic_pointer_cast<Player>(_objects[objectId]))
	{
		dynamic_pointer_cast<Player>(_objects[objectId])->SetHp(0);
	}
	else
	{
		RemoveMonster(objectId);
	}
}

void Room::SetRoomIndex(uint64 roomIndex)
{
	_roomIndex = roomIndex;
	info->set_room_id(roomIndex);	
}

void Room::UpdateTick()
{
	// TODO : 몬스터 이동, 공격
	if(_players.empty())
	{
		ReleaseThisRoom();
		return;
	}

	for(auto& monster : _monsters)
	{
		//monster.second->CalcDist();
		//monster.second->CanAttack();
		monster.second->UpdateBehaviourTree();
	}
}

RoomRef Room::GetRoomRef()
{
	return static_pointer_cast<Room>(shared_from_this());
}

void Room::ReleaseThisRoom()
{
	auto self = GetRoomRef();
	_lobby.lock()->RemoveRoom(self);

	_players.clear();
	_objects.clear();
	_monsters.clear();
	ClearJobs();
}

void Room::SpawnMonsters()
{
	for (int i = 0; i < 4; i++)
	{
		MonsterRef monster = ObjectUtils::CreateMonster();

		AddMonster(monster);
	}
}

void Room::SpawnMonster(const Protocol::PosInfo& pos_Info)
{
	MonsterRef monster = ObjectUtils::CreateMonster();

	if (AddMonster(monster, pos_Info))
	{
		Protocol::STC_SPAWN spawnPkt;

		Protocol::ObjectInfo* objInfo = new Protocol::ObjectInfo();
		objInfo->CopyFrom(monster->GetObjectInfo());

		Protocol::PosInfo* posInfo = new Protocol::PosInfo();
		posInfo->CopyFrom(pos_Info);

		objInfo->set_allocated_pos_info(posInfo);

		spawnPkt.set_allocated_object_info(objInfo);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer);
	}
}

void Room::Spawn(const Protocol::CreatureType creatureType, const Protocol::PosInfo& info)
{
	if (creatureType == Protocol::CreatureType::CREATURE_TYPE_MONSTER)
	{
		SpawnMonster(info);
	}
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

	_objects.erase(objectId);

	return true;
}

bool Room::AddPlayer(PlayerRef player)
{
	// Room에 있다면 문제가 있다.
	if (_players.find(player->GetPlayerInfo().player_id()) != _players.end())
	{
		return false;
	}

	Protocol::ObjectInfo objectInfo = Protocol::ObjectInfo();
	objectInfo.CopyFrom(player->GetObjectInfo());
	objectInfo.set_object_id(ObjectUtils::GetNewObjectID());
	objectInfo.mutable_pos_info()->set_object_id(objectInfo.object_id());

	player->SetObjectInfo(objectInfo);

	_players.insert(make_pair(player->GetPlayerInfo().player_id(), player));
	_objects.insert(make_pair(player->GetObjectInfo().object_id(), player));
	player->room.store(GetRoomRef());

	info->set_current_player_count(_players.size());
	info->add_players()->CopyFrom(player->GetPlayerInfo());

	return true;
}

bool Room::RemovePlayer(PlayerRef player, bool isExitGame)
{
	uint64 playerIndex = player->GetPlayerInfo().player_id();
	// 플레이어가 Room에 없으면 문제가 있다.
	if (_players.find(playerIndex) == _players.end())
	{
		return false;
	}
	
	if(isExitGame == false)
	{
		_lobby.lock()->AddPlayer(player);
	}

	info->clear_players();

	if (info->host().player_id() == playerIndex)
	{
		info->clear_host();
		auto originHost = _players.find(playerIndex);
		
		if (next(originHost) != _players.end())
		{ 
			// ���ο� ȣ��Ʈ ����
			Protocol::PlayerInfo* newHost = new Protocol::PlayerInfo();
			newHost->CopyFrom((next(originHost)->second->GetPlayerInfo()));
			info->set_allocated_host(newHost);
		}
	}

	_players.erase(playerIndex);
	info->set_current_player_count(_players.size());

	for (const auto& player : _players)
	{
		info->add_players()->CopyFrom((player.second->GetPlayerInfo()));
	}

	return true;
}

bool Room::AddMonster(MonsterRef monster, const Protocol::PosInfo& pos_Info)
{
	if (_monsters.size() >= 4)
	{
		return false;
	}

	if(_monsters.find(monster->GetObjectInfo().object_id()) != _monsters.end())
	{
		return false;
	}

	Protocol::ObjectInfo objectInfo = Protocol::ObjectInfo();
	objectInfo.CopyFrom(monster->GetObjectInfo());
	objectInfo.set_object_id(ObjectUtils::GetNewObjectID());

	Protocol::PosInfo posInfo = Protocol::PosInfo();
	posInfo.CopyFrom(pos_Info);
	posInfo.set_object_id(objectInfo.object_id());
	objectInfo.mutable_pos_info()->CopyFrom(posInfo);

	monster->SetObjectInfo(objectInfo);

	_monsters.insert(make_pair(monster->GetObjectInfo().object_id(), monster));
	_objects.insert(make_pair(monster->GetObjectInfo().object_id(), monster));

	monster->room.store(GetRoomRef());

	monster->Init();

	return true;
}

bool Room::RemoveMonster(uint64 monsterId)
{
	if(_monsters.find(monsterId) == _monsters.end())
	{
		return false;
	}

	_monsters.erase(monsterId);
	_objects.erase(monsterId);
	
	return true;
}

void Room::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& item : _objects)
	{
		PlayerRef player = dynamic_pointer_cast<Player>(item.second);
		if (player == nullptr)
			continue;

		if (player->GetObjectInfo().object_id() == exceptId)
			continue;

		if (GameSessionRef session = player->session.lock())
			session->Send(sendBuffer);
	}
}

void Room::BroadcastToPlayer(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& player : _players)
	{
		if (player.second->GetObjectInfo().object_id() == exceptId)
			continue;

		if (GameSessionRef session = player.second->GetSession())
			session->Send(sendBuffer);
	}
}
