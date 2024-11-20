#pragma once
#include <any>

enum class ENodeState
{
	Success,
	Failure,
	Abort,
	Running,
};

enum class EBlackboardKey
{
	Target,
	Position,
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

class BehaviourTree : public enable_shared_from_this<BehaviourTree>
{
public:
	BehaviourTree(shared_ptr<Monster> _owner);
	virtual ~BehaviourTree();

	ENodeState Update();

	static vector<shared_ptr<Node>> GetChildren(const std::shared_ptr<Node>& parent);

	static void Traverse(const std::shared_ptr<Node>& node, const std::function<void(const std::shared_ptr<Node>&)>& visitor);

	void Bind(any* context);
public:
	weak_ptr<class Monster> owner;

	shared_ptr<class RootNode> rootNode;
	vector<shared_ptr<Node>> nodes;

	shared_ptr<Blackboard> blackboard;

	atomic<ENodeState> treeState = ENodeState::Running;
};

class Blackboard : public enable_shared_from_this<Blackboard>
{
public:
	Blackboard() = default;
	virtual ~Blackboard();

public:
	weak_ptr<BehaviourTree> tree;
	map<EBlackboardKey, atomic<any>> data;
};

class Node : public enable_shared_from_this<Node>
{
public:
	Node();
	virtual ~Node();

	ENodeState Update();

	void Abort();

protected:
	virtual void OnStart() = 0;
	virtual void OnStop() = 0;
	virtual ENodeState OnUpdate() = 0;

public:
	weak_ptr<BehaviourTree> tree;
	shared_ptr<Blackboard> blackboard;
	weak_ptr<Node> parent;

	atomic<ENodeState> state = ENodeState::Running;

	bool started = false;

	atomic<any> context;
};

