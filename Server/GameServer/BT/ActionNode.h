#pragma once
#include "BehaviourTree.h"

class ActionNode : public Node
{
public:
	ActionNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : Node(_tree, _blackboard) {}
};

class WaitNode : public ActionNode
{
public:
	WaitNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : ActionNode(_tree, _blackboard) {}

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
	RandomPosition(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : ActionNode(_tree, _blackboard) {}

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

public:
	Vector2 min = { -600, -600 };
	Vector2 max = { 600, 600 };
};

class MoveToPosition : public ActionNode
{
public:
	MoveToPosition(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : ActionNode(_tree, _blackboard) {}

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;
};

class MoveToPlayer : public ActionNode
{
public:
	MoveToPlayer(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : ActionNode(_tree, _blackboard) {};

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;
};

class AttackNode : public ActionNode
{
public:
	AttackNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : ActionNode(_tree, _blackboard) {}

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

public:
	void SetAttackType(int _attackType) { attackType = _attackType; }
	void SetDamage(float _damage) { damage = _damage; }
	void SetHitRange(float _hitRange) { hitRange = _hitRange; }
	void SetHitAngle(float _hitAngle) { hitAngle = _hitAngle; }
	void SetIsRangeAttack(bool _isRangeAttack) { isRangeAttack = _isRangeAttack; }

	void SetDuration(float _duration) { duration = _duration; }

private:
	bool isRangeAttack = false;
	int attackType = 0;
	float damage = 20.f;
	float hitRange = 200.f;
	float hitAngle = 45.f;

	float duration = 2;
	float startTime;
};