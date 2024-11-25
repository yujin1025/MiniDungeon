#pragma once
#include "BehaviourTree.h"

enum class ENotifyObserver
{
	OnValueChange,
	OnResultChange,
};

enum class EFlowAbortMode
{
	None,
	Self,
	LowPriority,
	Both,
};

class DecoratorNode : public Node
{
public:
	DecoratorNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : Node(_tree, _blackboard) {}
	virtual ~DecoratorNode();

public:
	shared_ptr<Node> child;
};

class RepeaterNode : public DecoratorNode
{
public:
	RepeaterNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : DecoratorNode(_tree, _blackboard) {}

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

public:
	atomic<bool> restartOnSuccess = true;
	atomic<bool> restartOnFailure = false;
};

class RootNode : public RepeaterNode
{
public:
	RootNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : RepeaterNode(_tree, _blackboard)
	{
		restartOnSuccess = true;
		restartOnFailure = true;
	}

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;
};

class HasTargetDecorator : public DecoratorNode
{
public:
	HasTargetDecorator(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : DecoratorNode(_tree, _blackboard) {}

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

};

class NoTargetDecorator : public DecoratorNode
{
public:
	NoTargetDecorator(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : DecoratorNode(_tree, _blackboard) {}

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;
};

class CanAttackDecorator : public DecoratorNode
{
public:
	CanAttackDecorator(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : DecoratorNode(_tree, _blackboard) {}
protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

public:
	void SetAttackRange(float range) { AttackRange = range; }

private:
	float AttackRange = 300.0f;
};

class CanNotAttackDecorator : public DecoratorNode
{
public:
	CanNotAttackDecorator(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : DecoratorNode(_tree, _blackboard) {}

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

public:
	void SetAttackRange(float range) { AttackRange = range; }

private:
	float AttackRange = 300.0f;
};

class CheckHealthDecorator : public DecoratorNode
{
public:
	CheckHealthDecorator(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : DecoratorNode(_tree, _blackboard) {}

	void SetHealthRange(float min, float max)
	{
		minHealth = min;
		maxHealth = max;
	}

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

private:
	float minHealth = 0.0f;
	float maxHealth = 100.0f;
};