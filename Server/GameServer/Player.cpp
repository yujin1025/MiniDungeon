#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "Monster.h"

Player::Player()
{
	_isPlayer = true;
	playerInfo = new Protocol::PlayerInfo();
	Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
	objectInfo->set_creature_type(Protocol::CreatureType::CREATURE_TYPE_PLAYER);
	obj_info->CopyFrom(*objectInfo);
	
	playerInfo->set_allocated_object_info(obj_info);

	_hp = 100;
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

void Player::Attack(const Protocol::AttackInfo& attack_info)
{

	if (attack_info.player_type() == Protocol::PLAYER_TYPE_AURORA)
	{
		switch (attack_info.attack_type())
		{
		case 0:
			ProcessAttack(Protocol::PLAYER_TYPE_AURORA, attack_info.attack_type(), attack_info.damage(), 300.f, 30.f);
			break;
		case 1:
			ProcessAttack(Protocol::PLAYER_TYPE_AURORA, attack_info.attack_type(), attack_info.damage(), 500.f, 45.f);
			break;
		case 2:
			ProcessAttack(Protocol::PLAYER_TYPE_AURORA, attack_info.attack_type(), attack_info.damage(), 700.f, 60.f);
			break;
		}
	}
	else if(attack_info.player_type() == Protocol::PLAYER_TYPE_DRONGO)
	{
		switch (attack_info.attack_type())
		{
		case 0:
			ProcessAttack(Protocol::PLAYER_TYPE_DRONGO, attack_info.attack_type(), attack_info.damage(), 300.f, 50.f);
			break;
		case 1:
			ProcessAttack(Protocol::PLAYER_TYPE_DRONGO, attack_info.attack_type(), attack_info.damage(), 1000.f, 50.f);
			break;
		case 2:
			ProcessAttack(Protocol::PLAYER_TYPE_DRONGO, attack_info.attack_type(), attack_info.damage(), 1500.f, 85.f);
			break;
		}
	}
}

void Player::ProcessAttack(Protocol::PlayerType playerType, int32 skillType, float damage, float maxDistance, float maxAngle)
{
	RoomRef currentRoom = room.load().lock();
	if (!currentRoom)
	{
		return;
	}

	Protocol::STC_ATTACKED attackedPkt;
	attackedPkt.set_attacking_object_id(objectInfo->object_id());
	attackedPkt.set_attacking_skill_type(skillType);

	for (auto& monster : currentRoom->_monsters)
	{
		auto& monsterposInfo = monster.second->GetPosInfo();
		auto& currentPosInfo = objectInfo->pos_info();
		float distance = monster.second->DistanceTo(currentPosInfo);

		if (distance <= maxDistance)
		{
			Vector3 toTarget = Vector3(monsterposInfo.x() - currentPosInfo.x(), monsterposInfo.y() - currentPosInfo.y(), 0).Normalize();
			Vector3 forward = Vector3::CalculateForwardVector(currentPosInfo.yaw());

			// Dot Product를 사용하여 각도를 계산
			float dotProduct = Vector3::DotProduct(toTarget, forward);
			float clampedDot = std::clamp(dotProduct, -1.0f, 1.0f);
			float AngleDegrees = RadiansToDegrees(acos(clampedDot));

			// 각도 조건 확인
			if (abs(AngleDegrees) <= maxAngle)
			{
				float currentHp = monster.second->GetHp();
				monster.second->SetHp(currentHp - damage);

				Protocol::AttackedInfo* attackedInfo = attackedPkt.add_attacked_infos();
				attackedInfo->set_attacked_object_id(monster.second->GetObjectInfo().object_id());
				attackedInfo->set_attacked_object_current_hp(monster.second->GetHp());

				if (playerType == Protocol::PLAYER_TYPE_DRONGO)
				{
					// Skill Type 0과 1은 가장 가까운 몬스터만 타겟
					if (skillType == 0 || skillType == 1)
					{
						break; // 가장 가까운 몬스터를 타격한 뒤 종료
					}
				}
			}
		}
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(attackedPkt);
	currentRoom->Broadcast(sendBuffer);
}
