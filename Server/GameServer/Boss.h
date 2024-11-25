#pragma once
#include "Monster.h"

class Boss : public Monster
{
public:
	Boss();
	virtual ~Boss();

	void Init();

private:
	const float MaxHp = 400;
private:
	USE_LOCK;
};

