#include "pch.h"
#include "Creature.h"
#include "Room.h"

Creature::Creature()
{
	objectInfo->set_object_type(Protocol::ObjectType::OBJECT_TYPE_CREATURE);
}

Creature::~Creature()
{

}

void Creature::SetHp(float hp)
{
	WRITE_LOCK; 
	_hp = hp;

	if(_hp <= 0)
	{
		_hp = 0;
		room.load().lock()->HandleDead(objectInfo->object_id());
	}
}
