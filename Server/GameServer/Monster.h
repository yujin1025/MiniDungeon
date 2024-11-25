#pragma once
#include "Creature.h"
#include "Room.h"
#include "BehaviourTree.h"

class MonsterBlackboard : public Blackboard
{
public:
	MonsterBlackboard() = default;
	virtual ~MonsterBlackboard() = default;

	Vector3 moveToPosition;
};


class Monster : public Creature
{
public:
	Monster();
	virtual ~Monster();

	void Init();

	void CalcDist();
	void CanAttack();
	float DistanceTo(const Protocol::PosInfo& targetPos);

	Vector3 GetCurrentVector() 
	{ 
		READ_LOCK; 
		const Protocol::PosInfo& currentPos = objectInfo->pos_info();
		return Vector3{ currentPos.x(), currentPos.y(), currentPos.z() }; 
	}

	Protocol::CreatureType creatureType = Protocol::CreatureType::CREATURE_TYPE_MONSTER;

	const Vector3 SpawnVec{ 3390.f, 640.f, 178.f };
	const float SpawnYaw = 180.f;
	const float BossSight = 1500.f; //보스가 플레이어를 감지할 수 있는 범위
	const float BossLoseSight = 2000.f; //보스가 플레이어를 놓치는 거리
	const float BossSightAngle = 360.f; //보스가 감지할 수 있는 각도
	const float MaxHp = 100.f;
	const float Speed = 300.f;

	float CurrentHp;
	float Damage = 20;
	float CalacDist;

	bool IsPlayerAlive; //스가 추적하는 플레이어가 살아있는지 확인
	bool IsDead;
	bool IsFindPlayer;
	 
	atomic<shared_ptr<Player>> TargetPlayer; //보스가 추적하는 플레이어 객체
	atomic<bool> isAttacking = false;

protected:
	Protocol::MonsterInfo* monsterInfo;

public:
	const Protocol::MonsterInfo GetMonsterInfo() { READ_LOCK;  return *monsterInfo; }
	void SetMonsterInfo(const Protocol::MonsterInfo& monst_info) { WRITE_LOCK; this->monsterInfo->CopyFrom(monst_info); }

	const Protocol::ObjectInfo& GetObjectInfo() { READ_LOCK; return *objectInfo; }
	void SetObjectInfo(const Protocol::ObjectInfo& obj_Info);

	const Protocol::PosInfo& GetPosInfo() { READ_LOCK; return objectInfo->pos_info(); }
	void SetPosInfo(const Protocol::PosInfo& pos_Info);

	void SetMovementState(Protocol::MoveState state);

protected:
	shared_ptr<BehaviourTree> behaviourTree;
	shared_ptr<MonsterBlackboard> blackboard;

public:
	void UpdateBehaviourTree();

private:
	USE_LOCK;
};

