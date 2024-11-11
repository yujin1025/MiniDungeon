#pragma once
#include "Creature.h"
#include "Room.h"

class Monster : public Creature
{
public:
	Monster();
	virtual ~Monster();

	void CalcDist();
	void CanAttack();
	Vector3 GetDestination() { return Vector3{ 0,0,0 }; }

	Protocol::CreatureType creatureType = Protocol::CreatureType::CREATURE_TYPE_MONSTER;
	Protocol::MonsterInfo* monsterInfo;

	const Vector3 SpawnVec{ 3390.f, 640.f, 178.f };
	const float SpawnYaw = 180.f;
	const float BossSight = 1500.f; //보스가 플레이어를 감지할 수 있는 범위
	const float BossLoseSight = 2000.f; //보스가 플레이어를 놓치는 거리
	const float BossSightAngle = 360.f; //보스가 감지할 수 있는 각도
	const float MaxHp = 600.f;
	const float Speed = 300.f;

	float CurrentHp;
	float Damage = 20;
	float CalacDist;

	bool IsPlayerAlive; //스가 추적하는 플레이어가 살아있는지 확인
	bool IsDead;
	bool IsFindPlayer;
	 
	ObjectRef TrackedPlayer; //보스가 추적하는 플레이어 객체
};

