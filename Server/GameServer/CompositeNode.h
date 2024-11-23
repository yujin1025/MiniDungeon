#pragma once
#include "BehaviourTree.h"

class CompositeNode : public Node
{
public:
	CompositeNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : Node(_tree, _blackboard) {}
	virtual ~CompositeNode();

public:
	vector<shared_ptr<Node>> children;
};

class SequencerNode : public CompositeNode
{
public:
	SequencerNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : CompositeNode(_tree, _blackboard) {}

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

protected:
	uint32 current;
};

class SelectorNode : public CompositeNode
{
public:
	SelectorNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : CompositeNode(_tree, _blackboard) {}

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

protected:
	uint32 current;
};

class ParallelNode : public CompositeNode
{
public:
	ParallelNode(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard) : CompositeNode(_tree, _blackboard) {}
	virtual ~ParallelNode();

	void AbortRunningChildren();

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

public:
	vector<ENodeState> childrenLeftToExcute;
	uint32 successCount;
	uint32 failureCount;
};
