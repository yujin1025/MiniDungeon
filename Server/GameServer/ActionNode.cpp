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
	LOG_INFO();
	if(GetTickCount64() / 1000 - startTime >= duration)
	{
		return ENodeState::Success;
	}
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
	LOG_INFO();
	Vector3 moveToPosition = any_cast<Vector3>(blackboard->data[EBlackboardKey::Position]);
	moveToPosition.x = min.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max.x - min.x)));
	moveToPosition.z = min.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max.y - min.y)));
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
	LOG_INFO();
	auto ownerMonster = tree.lock()->owner.lock();
	if (!ownerMonster)
	{
		return ENodeState::Failure;
	}

	Vector3 moveToPosition = any_cast<Vector3>(blackboard->data[EBlackboardKey::Position]);
	//TODO : Move to position

	return ENodeState::Success;
}

void AttackNode::OnStart()
{
}

void AttackNode::OnStop()
{
}

ENodeState AttackNode::OnUpdate()
{
	LOG_INFO();
	auto ownerMonster = tree.lock()->owner.lock();
	if (!ownerMonster)
	{
		return ENodeState::Failure;
	}

	if(ownerMonster->isAttacking == false)
	{
		return ENodeState::Success;
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

		ownerMonster->isAttacking = true;
		return ENodeState::Running;
	}

	return ENodeState::Failure;
}
