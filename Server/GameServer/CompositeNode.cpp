#include "pch.h"
#include "CompositeNode.h"

void SequencerNode::OnStart()
{
	current = 0;
}

void SequencerNode::OnStop()
{
}

ENodeState SequencerNode::OnUpdate()
{
	for (int i = current; i < children.size(); i++)
	{
		current = i;
		auto child = children[i];

		switch (child->Update())
		{
		case ENodeState::Failure:
			return ENodeState::Failure;
		case ENodeState::Running:
			return ENodeState::Running;
		case ENodeState::Success:
			continue;
		}
	}

	return ENodeState::Success;
}

void SelectorNode::OnStart()
{
	current = 0;
}

void SelectorNode::OnStop()
{
}

ENodeState SelectorNode::OnUpdate()
{
	for (int i = current; i < children.size(); i++)
	{
		current = i;
		auto child = children[i];

		switch (child->Update())
		{
		case ENodeState::Success:
			return ENodeState::Success;
		case ENodeState::Running:
			return ENodeState::Running;
		case ENodeState::Failure:
			continue;
		}
	}

	return ENodeState::Failure;
}

void ParallelNode::OnStart()
{
	childrenLeftToExcute.clear();
	for(auto & child : children)
	{
		childrenLeftToExcute.push_back(ENodeState::Running);
	}
}

void ParallelNode::OnStop()
{
}

ENodeState ParallelNode::OnUpdate()
{
	bool stillRunning = false;

	for(int i = 0; i < childrenLeftToExcute.size(); i++)
	{
		if(childrenLeftToExcute[i] == ENodeState::Running)
		{
			auto status = children[i]->Update();
			if (status == ENodeState::Failure)
			{
				AbortRunningChildren();
				return ENodeState::Failure;
			}
		}

		if(childrenLeftToExcute[i] == ENodeState::Running)
		{
			stillRunning = true;
		}
		
		childrenLeftToExcute[i] = ENodeState::Success;
	}

	return stillRunning ? ENodeState::Running : ENodeState::Success;
}

void ParallelNode::AbortRunningChildren()
{
	for (int i = 0; i < childrenLeftToExcute.size(); i++)
	{
		if (childrenLeftToExcute[i] == ENodeState::Running)
		{
			children[i]->Abort();
		}
	}
}
