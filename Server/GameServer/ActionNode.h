#pragma once
#include "BehaviourTree.h"

class ActionNode : public Node
{
public:
	ActionNode() = default;
	virtual ~ActionNode() = default;
};

class WaitNode : public ActionNode
{
public:
	WaitNode() = default;
	virtual ~WaitNode() = default;

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

public:
	float duration = 2;
	float startTime;
};

class RandomPosition : public ActionNode
{
public:
	RandomPosition() = default;
	virtual ~RandomPosition() = default;

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

public:
	Vector2 min = { -10, -10 };
	Vector2 max = { 10, 10 };
};

class MoveToPosition : public ActionNode
{
public:
	MoveToPosition() = default;
	virtual ~MoveToPosition() = default;

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;
};

class AttackNode : public ActionNode
{
public:
	AttackNode() = default;
	virtual ~AttackNode() = default;

	void SetIsAttackSuccessful(bool value) { isAttackSuccessful = value; }

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

private:
	atomic<bool> isAttackSuccessful = false;
};