#pragma once
#include "BehaviourTree.h"

class ServiceNode : public Node
{
public:
	ServiceNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : Node(_tree, _blackboard) {}
	virtual ~ServiceNode();

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

	virtual void OnUpdateService() = 0;

protected:
	float lastUpdateTime = 0.0f;
	float updateInterval = 0.2f;

public:
	shared_ptr<Node> child;
};

class DetectionService : public ServiceNode
{
public:
	DetectionService(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : ServiceNode(_tree, _blackboard) {}

protected:
	virtual void OnUpdateService() override;

private:
	float detectRange = 600.0f;
};
