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
	DecoratorNode() = default;
	virtual ~DecoratorNode();

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

public:
	shared_ptr<Node> child;
};

class RepeaterNode : public DecoratorNode
{
public:
	RepeaterNode() = default;
	virtual ~RepeaterNode() = default;

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
	RootNode();
	virtual ~RootNode() = default;

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;
};

class HasTargetDecorator : public DecoratorNode
{
public:
	HasTargetDecorator() = default;
	virtual ~HasTargetDecorator() = default;

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

};

class NoTargetDecorator : public DecoratorNode
{
public:
	NoTargetDecorator() = default;
	virtual ~NoTargetDecorator() = default;

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;
};

class CanAttackDecorator : public DecoratorNode
{
public:
	CanAttackDecorator() = default;
	virtual ~CanAttackDecorator() = default;

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

private:
	float AttackRange = 300.0f;
};

class CanNotAttackDecorator : public DecoratorNode
{
public:
	CanNotAttackDecorator() = default;
	virtual ~CanNotAttackDecorator() = default;

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

private:
	float AttackRange = 300.0f;
};