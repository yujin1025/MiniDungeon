#include "pch.h"
#include "ActionNode.h"
#include "Monster.h"
#include "Player.h"

void WaitNode::OnStart()
{
	startTime = GetTickCount64() / 1000;
}

void WaitNode::OnStop()
{
}

ENodeState WaitNode::OnUpdate()
{
	if(GetTickCount64() / 1000 - startTime >= duration)
	{
		LOG("Wait Over");
		return ENodeState::Success;
	}

	LOG("Waiting");
	return ENodeState::Running;
}

void RandomPosition::OnStart()
{
}

void RandomPosition::OnStop()
{
}

ENodeState RandomPosition::OnUpdate()
{
	if(blackboard == nullptr)
	{
		LOG("Get Random Position Failed : blackboard is nullptr");
		return ENodeState::Failure;
	}

	Vector3 moveToPosition = any_cast<Vector3>(blackboard->GetData(EBlackboardKey::Position));
	moveToPosition.x = min.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max.x - min.x)));
	moveToPosition.z = min.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max.y - min.y)));
	blackboard->SetData(EBlackboardKey::Position, moveToPosition);

	LOG("Get Random Position");
	return ENodeState::Success;
}

void MoveToPosition::OnStart()
{
}

void MoveToPosition::OnStop()
{
}

ENodeState MoveToPosition::OnUpdate()
{
	auto bt = tree.lock();
	if (bt == nullptr)
	{
		LOG("MoveToPosition Failed : bt is nullptr");
		return ENodeState::Failure;
	}


	auto ownerMonster = bt->owner.lock();
	if (ownerMonster == nullptr)
	{
		LOG("MoveToPosition Failed : ownerMonster is nullptr");
		return ENodeState::Failure;
	}


	if (blackboard == nullptr)
	{
		LOG("MoveToPosition Failed : blackboard is nullptr");
		return ENodeState::Failure;
	}

	Vector3 moveToPosition = any_cast<Vector3>(blackboard->GetData(EBlackboardKey::Position));
	LOGF("MoveToPosition : %f, %f, %f", moveToPosition.x, moveToPosition.y, moveToPosition.z);
	//TODO : Move to position

	return ENodeState::Success;
}

void AttackNode::OnStart()
{
	auto bt = tree.lock();
	if (bt == nullptr)
	{
		LOG("AttackNode bt is nullptr");
		return;
	}

	auto ownerMonster = bt->owner.lock();
	if (ownerMonster == nullptr)
	{
		LOG("AttackNode ownerMonster is nullptr");
		return;
	}

	auto target = ownerMonster->TargetPlayer.load();
	if (target)
	{
		Protocol::STC_MONSTER_ATTACK pkt;
		pkt.set_monster_id(ownerMonster->GetObjectInfo().object_id());
		pkt.set_target_id(target->GetObjectInfo().object_id());

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		auto room = ownerMonster->room.load().lock();
		if (room)
		{
			room->Broadcast(sendBuffer);
		}

		LOG("Attack Start");
		ownerMonster->isAttacking = true;
		startTime = GetTickCount64() / 1000;
		return;
	}

	LOG("AttackNode target is nullptr");
	return;
}

void AttackNode::OnStop()
{
}

ENodeState AttackNode::OnUpdate()
{
	if (GetTickCount64() / 1000 - startTime >= duration)
	{
		LOG("Attack Finished");
		return ENodeState::Success;
	}

	auto bt = tree.lock();
	if (bt == nullptr)
	{
		LOG("Attack Failed : bt is nullptr");
		return ENodeState::Failure;
	}

	auto ownerMonster = bt->owner.lock();
	if (ownerMonster == nullptr)
	{
		LOG("Attack Failed : ownerMonster is nullptr");
		return ENodeState::Failure;
	}

	auto target = ownerMonster->TargetPlayer.load();
	if (target)
	{
		LOG("Attacking")
		return ENodeState::Running;
	}

	LOG("Attack Failed : target is nullptr");
	return ENodeState::Failure;
}
