#include "pch.h"
#include "Monster.h"

Monster::Monster()
{
	objectInfo = new Protocol::ObjectInfo();
	monsterInfo = new Protocol::MonsterInfo();
}

Monster::~Monster()
{
	delete objectInfo;
	delete monsterInfo;
}

void Monster::CalcDist()
{
}

void Monster::CanAttack()
{
}
