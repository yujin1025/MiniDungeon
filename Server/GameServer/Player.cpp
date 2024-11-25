#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "Monster.h"
#include "Boss.h"

Player::Player()
{
	_isPlayer = true;
	playerInfo = new Protocol::PlayerInfo();
	Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
	objectInfo->set_creature_type(Protocol::CreatureType::CREATURE_TYPE_PLAYER);
	obj_info->CopyFrom(*objectInfo);
	
	playerInfo->set_allocated_object_info(obj_info);

	_hp = 300;
}

Player::~Player()
{
	delete playerInfo;
	playerInfo = nullptr;
}

void Player::SetPlayerType(Protocol::PlayerType playerType)
{
	WRITE_LOCK; 
	playerInfo->set_player_type(playerType);
	switch (playerType)
	{
	case Protocol::PLAYER_TYPE_AURORA:
		_hp = 300;
		break;
	case Protocol::PLAYER_TYPE_DRONGO:
		_hp = 200;
		break;
	}
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
			ProcessAttack(Protocol::PLAYER_TYPE_DRONGO, attack_info.attack_type(), attack_info.damage(), 1200.f, 70.f);
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
	attackedPkt.set_attacking_player_type(playerType);
	attackedPkt.set_attacking_skill_type(skillType);

	float closestDistance = FLT_MAX; // 가장 가까운 몬스터 거리
	MonsterRef closestMonster;      // 가장 가까운 몬스터의 참조
	for (auto& monster : currentRoom->GetMonsters())
	{
		MonsterRef attackedMon = monster.second;
		const auto& monsterposInfo = attackedMon->GetPosInfo();
		const auto& currentPosInfo = objectInfo->pos_info();
		float distance = attackedMon->DistanceTo(currentPosInfo);

		if (distance <= maxDistance)
		{
			Vector3 toTarget = Vector3(monsterposInfo.x() - currentPosInfo.x(), monsterposInfo.y() - currentPosInfo.y(), 0).Normalize();
			Vector3 forward = Vector3::CalculateForwardVector(currentPosInfo.yaw());

			// Dot Product를 사용하여 각도를 계산
			float dotProduct = Vector3::DotProduct(toTarget, forward);
			float clampedDot = std::clamp(dotProduct, -1.0f, 1.0f);
			float AngleDegrees = RadiansToDegrees(acos(clampedDot));

			// 각도 조건 확인
			if (AngleDegrees <= maxAngle)
			{
				// SkillType이 0 또는 1일 경우 가장 가까운 몬스터만 타겟
				if (playerType == Protocol::PLAYER_TYPE_DRONGO && (skillType == 0 || skillType == 1))
				{
					if (distance < closestDistance)
					{
						closestDistance = distance;
						closestMonster = attackedMon;
					}
				}
				else
				{
					// SkillType이 0 또는 1이 아닌 경우 모든 몬스터 처리
					float currentHp = attackedMon->GetHp();
					attackedMon->SetHp(currentHp - damage);

					Protocol::AttackedInfo* attackedInfo = attackedPkt.add_attacked_infos();
					attackedInfo->set_attacked_object_id(attackedMon->GetObjectInfo().object_id());
					attackedInfo->set_attacked_object_current_hp(attackedMon->GetHp());
				}
			}
		}
	}

	if (closestMonster)
	{
		float currentHp = closestMonster->GetHp();
		closestMonster->SetHp(currentHp - damage);

		Protocol::AttackedInfo* attackedInfo = attackedPkt.add_attacked_infos();
		attackedInfo->set_attacked_object_id(closestMonster->GetObjectInfo().object_id());
		attackedInfo->set_attacked_object_current_hp(closestMonster->GetHp());
	}

	if (currentRoom->GetBoss() != nullptr)
	{
		BossRef boss = currentRoom->GetBoss();
		const auto& bossPosInfo = boss->GetPosInfo();
		const auto& currentPosInfo = objectInfo->pos_info();
		float distance = boss->DistanceTo(currentPosInfo);

		if (distance <= maxDistance)
		{
			Vector3 toTarget = Vector3(bossPosInfo.x() - currentPosInfo.x(), bossPosInfo.y() - currentPosInfo.y(), 0).Normalize();
			Vector3 forward = Vector3::CalculateForwardVector(currentPosInfo.yaw());

			// Dot Product를 사용하여 각도를 계산
			float dotProduct = Vector3::DotProduct(toTarget, forward);
			float clampedDot = std::clamp(dotProduct, -1.0f, 1.0f);
			float AngleDegrees = RadiansToDegrees(acos(clampedDot));

			// 각도 조건 확인
			if (AngleDegrees <= maxAngle)
			{
				float currentHp = boss->GetHp();
				boss->SetHp(currentHp - damage);

				Protocol::AttackedInfo* attackedInfo = attackedPkt.add_attacked_infos();
				attackedInfo->set_attacked_object_id(boss->GetObjectInfo().object_id());
				attackedInfo->set_attacked_object_current_hp(boss->GetHp());
			}
		}
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(attackedPkt);
	currentRoom->Broadcast(sendBuffer);
}
