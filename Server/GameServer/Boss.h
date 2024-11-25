#pragma once
#include "Monster.h"

class Boss : public Monster
{
public:
	Boss();
	virtual ~Boss();

	void Init();

private:
	USE_LOCK;
};

