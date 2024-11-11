#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "Monster.h"
#include "GameSession.h"

atomic<uint64> ObjectUtils::s_idGenerator = 1;

PlayerRef ObjectUtils::CreatePlayer(GameSessionRef session)
{
	PlayerRef player = make_shared<Player>();
	player->session = session;
	session->player.store(player);

	return player;
}

MonsterRef ObjectUtils::CreateMonster()
{
	const int64 newId = s_idGenerator.fetch_add(1);

	MonsterRef monster = make_shared<Monster>();
	monster->CurrentHp = monster->MaxHp;

	monster->posInfo->set_object_id(newId);
	monster->posInfo->set_x(monster->SpawnVec.x);
	monster->posInfo->set_y(monster->SpawnVec.y);
	monster->posInfo->set_z(monster->SpawnVec.z);
	monster->posInfo->set_yaw(monster->SpawnYaw);

	Protocol::PosInfo* pos = new Protocol::PosInfo();
	pos->CopyFrom(*monster->posInfo);

	monster->objectInfo->set_object_id(newId);
	monster->objectInfo->set_creature_type(monster->creatureType);
	monster->objectInfo->set_allocated_pos_info(pos);

	Protocol::ObjectInfo* objInfo = new Protocol::ObjectInfo();
	objInfo->CopyFrom(*monster->objectInfo);

	monster->monsterInfo->set_allocated_object_info(objInfo);
	monster->monsterInfo->set_object_type(Protocol::ObjectType::OBJECT_TYPE_CREATURE);
	monster->monsterInfo->set_monster_hp(monster->MaxHp);
	monster->monsterInfo->set_speed(monster->Speed);
	monster->monsterInfo->set_damage(monster->Damage);

	return monster;
}

const uint64 ObjectUtils::GetNewObjectID()
{
	return s_idGenerator.fetch_add(1);
}
