#pragma once
#include "Object.h"

class Creature : public Object
{
public:
	Creature();
	virtual ~Creature();

public:
	const float GetHp(){ READ_LOCK; return _hp; }
	void SetHp(float hp); 
protected:
	float _hp;

private:
	USE_LOCK;
};

