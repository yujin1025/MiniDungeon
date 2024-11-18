#pragma once

enum class ENodeState
{
	Running,
	Success,
	Failure
};

struct Vector3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

struct Vector2
{
	float x = 0.0f;
	float y = 0.0f;
};

class BehaviourTree
{
public:
	BehaviourTree() = default;
	virtual ~BehaviourTree() = default;

	ENodeState Update();

	static vector<shared_ptr<Node>> GetChildren(const std::shared_ptr<Node>& parent);

	static void Traverse(const std::shared_ptr<Node>& node, const std::function<void(const std::shared_ptr<Node>&)>& visitor);

	shared_ptr<BehaviourTree> Clone() const;

	void Bind(void* context);
public:
	shared_ptr<Node> rootNode;
	ENodeState treeState = ENodeState::Running;
	vector<shared_ptr<Node>> nodes;
	shared_ptr<Blackboard> blackboard;
};

class Blackboard
{
public:
	Blackboard() = default;
	virtual ~Blackboard() = default;

	Vector3 moveToPosition;
};

class Node : public enable_shared_from_this<Node>
{
public:
	Node() = default;
	virtual ~Node() = default;

	ENodeState Update();

	void Abort();

protected:
	virtual void OnStart() = 0;
	virtual void OnStop() = 0;
	virtual ENodeState OnUpdate() = 0;

public:
	ENodeState state = ENodeState::Running;
	bool started = false;
	shared_ptr<Blackboard> blackboard;
	void* context = nullptr;
};

class RootNode : public Node
{
public:
	RootNode() = default;
	virtual ~RootNode() = default;

protected:
	virtual void OnStart() override;
	virtual void OnStop() override;
	virtual ENodeState OnUpdate() override;

public:
	shared_ptr<Node> child;
};



