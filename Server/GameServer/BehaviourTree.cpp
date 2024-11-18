#include "pch.h"
#include "BehaviourTree.h"

ENodeState BehaviourTree::Update()
{
	if (rootNode && rootNode->state == ENodeState::Running)
	{
		treeState = rootNode->Update();
	}

	return treeState;
}

vector<shared_ptr<Node>> BehaviourTree::GetChildren(const std::shared_ptr<Node>& parent)
{
	return vector<shared_ptr<Node>>();
}

void BehaviourTree::Traverse(const std::shared_ptr<Node>& node, const std::function<void(const std::shared_ptr<Node>&)>& visitor)
{
	if (node)
	{
		visitor(node);

		auto children = GetChildren(node);
		for (auto& child : children)
		{
			Traverse(child, visitor);
		}
	}
}

shared_ptr<BehaviourTree> BehaviourTree::Clone() const
{
	auto tree = make_shared<BehaviourTree>();
	tree->rootNode = rootNode;
	tree->nodes = nodes;

	Traverse(rootNode, [&](const shared_ptr<Node>& node)
		{
			tree->nodes.push_back(node);
		});

	return tree;
}

void BehaviourTree::Bind(void* context)
{
	Traverse(rootNode, [&](const shared_ptr<Node>& node)
		{
			node->context = context;
			node->blackboard = blackboard;
		});
}


ENodeState Node::Update()
{
	if (!started)
	{
		OnStart();
		started = true;
	}

	state = OnUpdate();

	if (state != ENodeState::Running)
	{
		OnStop();
		started = false;
	}

	return state;
}

void Node::Abort()
{
	BehaviourTree::Traverse(shared_from_this(), [&](const shared_ptr<Node>& node)
		{
			if (node->state == ENodeState::Running)
			{
				node->started = false;
				node->state = ENodeState::Running;
				node->OnStop();
			}
		});
}

void RootNode::OnStart()
{
}

void RootNode::OnStop()
{
}

ENodeState RootNode::OnUpdate()
{
	return child->Update();
}

