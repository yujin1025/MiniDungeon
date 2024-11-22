#pragma once

class Node;
class Blackboard;

class BehaviourTree : public enable_shared_from_this<BehaviourTree>
{
public:
	BehaviourTree();
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
private:
	USE_LOCK;
	map<EBlackboardKey, any> data;

public:
	void SetData(EBlackboardKey key, any value) 
	{
		WRITE_LOCK;
		data[key] = move(value);
	}

	any GetData(EBlackboardKey key) 
	{
		WRITE_LOCK;
		auto it = data.find(key);
		if (it != data.end()) 
		{
			return it->second;
		}
		return GetDefaultValue(key);
	}

	any GetDefaultValue(EBlackboardKey key)
	{
		switch (key)
		{
		case EBlackboardKey::Target:
			return 0;  // Target은 uint64_t 타입 기본값.
		case EBlackboardKey::Position:
			return Vector3{ 0.0f, 0.0f, 0.0f };  // Vector3 타입 기본값.
		default:
			return {};  // 기본적으로 비어 있는 std::any 반환.
		}
	}
};

class Node : public enable_shared_from_this<Node>
{
public:
	Node(shared_ptr<BehaviourTree> _tree, shared_ptr<Blackboard> _blackboard)
	: tree(_tree), blackboard(_blackboard), started(false), state(ENodeState::Running) {}

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
};

