#pragma once
#include "BehaviourTree.h"

class CompositeNode : public Node
{
public:
	CompositeNode() = default;
	virtual ~CompositeNode() = default;

public:
	vector<shared_ptr<Node>> children;
};

class SequencerNode : public CompositeNode
{
public:
	SequencerNode() = default;
	virtual ~SequencerNode() = default;

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
	SelectorNode() = default;
	virtual ~SelectorNode() = default;

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
	ParallelNode() = default;
	virtual ~ParallelNode() = default;

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
