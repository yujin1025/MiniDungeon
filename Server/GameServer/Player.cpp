#include "pch.h"
#include "Player.h"

Player::Player()
{
	_isPlayer = true;
	playerInfo = new Protocol::PlayerInfo();
	Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
	objectInfo->set_creature_type(Protocol::CreatureType::CREATURE_TYPE_PLAYER);
	obj_info->CopyFrom(*objectInfo);
	
	playerInfo->set_allocated_object_info(obj_info);
}

Player::~Player()
{
	delete playerInfo;
	playerInfo = nullptr;
}

void Player::SetObjectInfo(const Protocol::ObjectInfo& obj_Info)
{
	WRITE_LOCK;
	Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
	obj_info->CopyFrom(obj_Info);
	
	playerInfo->set_allocated_object_info(obj_info);
	objectInfo->CopyFrom(playerInfo->object_info());
}

void Player::SetPosInfo(const Protocol::PosInfo& pos_Info)
{
	WRITE_LOCK;
	Protocol::PosInfo* pos_info = new Protocol::PosInfo();
	pos_info->CopyFrom(pos_Info);
	
	Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
	obj_info->CopyFrom(*objectInfo);
	obj_info->set_allocated_pos_info(pos_info);

	playerInfo->set_allocated_object_info(obj_info);
	objectInfo->CopyFrom(playerInfo->object_info());
}
