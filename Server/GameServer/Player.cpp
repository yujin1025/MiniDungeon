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
	RoomRef currentRoom = room.load().lock();
	switch (attack_info.attack_type())
	{
		case 0:
		{
			Protocol::STC_ATTACKED attackedPkt;
			attackedPkt.set_attacking_object_id(objectInfo->object_id());
			attackedPkt.set_attacking_skill_type(0);
			float minDistance = FLT_MAX;
			MonsterRef closestMonster;
			for (auto& monster : currentRoom->_monsters)
			{
				auto& monsterposInfo = monster.second->GetPosInfo();
				auto& currentPosInfo = objectInfo->pos_info();
				float distance = monster.second->DistanceTo(currentPosInfo);

				if (distance <= 300.f)
				{
					Vector3 toTarget = Vector3(monsterposInfo.x() - currentPosInfo.x(), monsterposInfo.y() - currentPosInfo.y(), 0).Normalize();
					Vector3 forward = Vector3::CalculateForwardVector(currentPosInfo.yaw());

					// Dot Product를 사용하여 각도를 계산
					float dotProduct = Vector3::DotProduct(toTarget, forward);
					float clampedDot = clamp(dotProduct, -1.0f, 1.0f);
					float AngleDegrees = RadiansToDegrees(acos(clampedDot));

					// 60도 안에 있는지 확인하고 가장 가까운 몬스터 지정
					if (abs(AngleDegrees) <= 30.0f && distance < minDistance)
					{
						minDistance = distance;
						closestMonster = monster.second;
					}
				}
			}

			if (closestMonster)
			{
				closestMonster->SetHp(closestMonster->GetHp() - attack_info.damage());

				Protocol::AttackedInfo* attackedInfo = attackedPkt.add_attacked_infos();
				attackedInfo->set_attacked_object_id(closestMonster->GetObjectInfo().object_id());
				attackedInfo->set_attacked_obejct_current_hp(closestMonster->GetHp());
			}
			SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(attackedPkt);
			currentRoom->Broadcast(sendBuffer);
			break;
		}
		case 1:
		{
			Protocol::STC_ATTACKED attackedPkt;
			attackedPkt.set_attacking_object_id(objectInfo->object_id());
			attackedPkt.set_attacking_skill_type(1);

			float minDistance = FLT_MAX;
			MonsterRef closestMonster;
			for (auto& monster : currentRoom->_monsters)
			{
				auto& monsterposInfo = monster.second->GetPosInfo();
				auto& currentPosInfo = objectInfo->pos_info();
				float distance = monster.second->DistanceTo(currentPosInfo);

				if (distance <= 500.f)
				{
					Vector3 toTarget = Vector3(monsterposInfo.x() - currentPosInfo.x(), monsterposInfo.y() - currentPosInfo.y(), 0).Normalize();
					Vector3 forward = Vector3::CalculateForwardVector(currentPosInfo.yaw());

					// Dot Product를 사용하여 각도를 계산
					float dotProduct = Vector3::DotProduct(toTarget, forward);
					float clampedDot = clamp(dotProduct, -1.0f, 1.0f);
					float AngleDegrees = RadiansToDegrees(acos(clampedDot));

					// 90도 안에 있는지 확인하고 가장 가까운 몬스터 지정
					if (abs(AngleDegrees) <= 45.0f && distance < minDistance)
					{
						minDistance = distance;
						closestMonster = monster.second;
					}
				}
			}

			if (closestMonster)
			{
				closestMonster->SetHp(closestMonster->GetHp() - attack_info.damage());

				Protocol::AttackedInfo* attackedInfo = attackedPkt.add_attacked_infos();
				attackedInfo->set_attacked_object_id(closestMonster->GetObjectInfo().object_id());
				attackedInfo->set_attacked_obejct_current_hp(closestMonster->GetHp());
			}
			SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(attackedPkt);
			currentRoom->Broadcast(sendBuffer);
			break;
		}
		case 2:
		{
			Protocol::STC_ATTACKED attackedPkt;
			attackedPkt.set_attacking_object_id(objectInfo->object_id());
			attackedPkt.set_attacking_skill_type(2);
			for (auto& monster : currentRoom->_monsters)
			{
				auto& monsterposInfo = monster.second->GetPosInfo();
				auto& currentPosInfo = objectInfo->pos_info();
				float distance = monster.second->DistanceTo(currentPosInfo);

				if (distance <= 700.f)
				{
					Vector3 toTarget = Vector3(monsterposInfo.x() - currentPosInfo.x(), monsterposInfo.y() - currentPosInfo.y(), 0).Normalize();
					Vector3 forward = Vector3::CalculateForwardVector(currentPosInfo.yaw());

					// Dot Product를 사용하여 각도를 계산
					float dotProduct = Vector3::DotProduct(toTarget, forward);
					float clampedDot = clamp(dotProduct, -1.0f, 1.0f);
					float AngleDegrees = RadiansToDegrees(acos(clampedDot));

					// 120도 안에 있는 모든 몬스터 피격
					if (abs(AngleDegrees) <= 60.0f)
					{
						float currentHp = monster.second->GetHp();
						monster.second->SetHp(currentHp - attack_info.damage());

						Protocol::AttackedInfo* attackedInfo = attackedPkt.add_attacked_infos();
						attackedInfo->set_attacked_object_id(monster.second->GetObjectInfo().object_id());
						attackedInfo->set_attacked_obejct_current_hp(monster.second->GetHp());
					}
				}
			}
			SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(attackedPkt);
			currentRoom->Broadcast(sendBuffer);
		}
	}
}
