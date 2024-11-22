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
	auto bt = tree.lock();
	if (bt == nullptr)
	{
		LOG("Get Random Position Failed : bt is nullptr");
		return ENodeState::Failure;
	}


	auto ownerMonster = bt->owner.lock();
	if (ownerMonster == nullptr)
	{
		LOG("Get Random Position Failed : ownerMonster is nullptr");
		return ENodeState::Failure;
	}

	Vector3 currentPos = ownerMonster->GetCurrentVector();

	if (currentPos == Vector3(0, 0 ,0))
	{
		LOG("Get Random Position Failed : ownerMonster current position is 0,0,0");
		return ENodeState::Failure;
	}

	if(blackboard == nullptr)
	{
		LOG("Get Random Position Failed : blackboard is nullptr");
		return ENodeState::Failure;
	}

	Vector3 moveToPosition = any_cast<Vector3>(blackboard->GetData(EBlackboardKey::Position));
	moveToPosition.x = currentPos.x + min.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max.x - min.x)));
	moveToPosition.y = currentPos.y + min.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max.y - min.y)));
	moveToPosition.z = currentPos.z;
	blackboard->SetData(EBlackboardKey::Position, moveToPosition);

	LOG("Get Random Position");
	return ENodeState::Success;
}

void MoveToPosition::OnStart()
{
	auto bt = tree.lock();
	if (bt == nullptr)
	{
		LOG("MoveToPosition Failed : bt is nullptr");
		return;
	}

	auto ownerMonster = bt->owner.lock();
	if (ownerMonster == nullptr)
	{
		LOG("MoveToPosition Failed : ownerMonster is nullptr");
		return;
	}

	if (blackboard == nullptr)
	{
		LOG("MoveToPosition Failed : blackboard is nullptr");
		return;
	}

	Vector3 moveToPosition = any_cast<Vector3>(blackboard->GetData(EBlackboardKey::Position));
	
	if(moveToPosition == ownerMonster->GetCurrentVector())
	{
		LOG("MoveToPosition Failed : moveToPosition is same as current position");
		return;
	}

	Protocol::STC_MOVE pkt;
	Protocol::PosInfo* posinfo = new Protocol::PosInfo();

	ownerMonster->SetMovementState(Protocol::MOVE_STATE_RUN);

	posinfo->set_object_id(ownerMonster->GetObjectInfo().object_id());
	posinfo->set_x(moveToPosition.x);
	posinfo->set_y(moveToPosition.y);
	posinfo->set_z(moveToPosition.z);
	posinfo->set_state(Protocol::MOVE_STATE_RUN);
	posinfo->set_yaw(ownerMonster->GetPosInfo().yaw());

	pkt.set_allocated_info(posinfo);
	pkt.set_target_object_id(0);

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	auto room = ownerMonster->room.load().lock();
	if (room)
	{
		room->Broadcast(sendBuffer);
	}
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

	if (ownerMonster->GetPosInfo().state() != Protocol::MOVE_STATE_RUN)
	{
		return ENodeState::Success;
	}

	if (blackboard == nullptr)
	{
		LOG("MoveToPosition Failed : blackboard is nullptr");
		return ENodeState::Failure;
	}

	return ENodeState::Running;
}


void MoveToPlayer::OnStart()
{
	auto bt = tree.lock();
	if (bt == nullptr)
	{
		LOG("MoveToPlayer Failed : bt is nullptr");
		return;
	}

	auto ownerMonster = bt->owner.lock();
	if (ownerMonster == nullptr)
	{
		LOG("MoveToPlayer Failed : ownerMonster is nullptr");
		return;
	}

	if (blackboard == nullptr)
	{
		LOG("MoveToPlayer Failed : blackboard is nullptr");
		return;
	}

	uint64 target_id = any_cast<uint64>(blackboard->GetData(EBlackboardKey::Target));

	if (target_id == 0)
	{
		LOG("MoveToPlayer Failed : There is no target");
		return;
	}

	Protocol::STC_MOVE pkt;
	Protocol::PosInfo* posinfo = new Protocol::PosInfo();
	ownerMonster->SetMovementState(Protocol::MOVE_STATE_RUN);

	posinfo->set_object_id(ownerMonster->GetObjectInfo().object_id());
	pkt.set_allocated_info(posinfo);
	pkt.set_target_object_id(target_id);

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	auto room = ownerMonster->room.load().lock();
	if (room)
	{
		room->Broadcast(sendBuffer);
	}
}

void MoveToPlayer::OnStop()
{
}

ENodeState MoveToPlayer::OnUpdate()
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

	if (ownerMonster->GetPosInfo().state() != Protocol::MOVE_STATE_RUN)
	{
		return ENodeState::Success;
	}

	if (blackboard == nullptr)
	{
		LOG("MoveToPosition Failed : blackboard is nullptr");
		return ENodeState::Failure;
	}

	return ENodeState::Running;
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

		ownerMonster->SetMovementState(Protocol::MOVE_STATE_SKILL);
		pkt.set_monster_id(ownerMonster->GetObjectInfo().object_id());
		pkt.set_target_id(target->GetObjectInfo().object_id());

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		auto room = ownerMonster->room.load().lock();
		if (room)
		{
			room->Broadcast(sendBuffer);
		}

		LOG("Attack Start");
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

	if (ownerMonster->GetPosInfo().state() != Protocol::MOVE_STATE_SKILL)
	{
		LOG("Attack Finished");
		return ENodeState::Success;
	}

	auto target = ownerMonster->TargetPlayer.load();
	if (target)
	{
		LOG("Attacking");
		return ENodeState::Running;
	}

	LOG("Attack Failed : target is nullptr");
	return ENodeState::Failure;
}
