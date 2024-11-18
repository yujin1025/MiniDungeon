#include "pch.h"
#include "ActionNode.h"

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
	blackboard->moveToPosition.x = min.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max.x - min.x)));
	blackboard->moveToPosition.z = min.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max.y - min.y)));
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
	return ENodeState();
}
