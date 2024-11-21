#include "pch.h"
#include "BehaviourTree.h"
#include "CompositeNode.h"
#include "DecoratorNode.h"

BehaviourTree::BehaviourTree()
{
	treeState = ENodeState::Running;
}

BehaviourTree::~BehaviourTree()
{
	owner.reset();
	rootNode.reset();
	nodes.clear();
	blackboard.reset();
}

ENodeState BehaviourTree::Update()
{
	if (rootNode && rootNode->state == ENodeState::Running)
	{
		treeState = rootNode->Update();
	}

	return treeState;
}

vector<shared_ptr<Node>> BehaviourTree::GetChildren(const shared_ptr<Node>& parent)
{
	auto composite = dynamic_pointer_cast<CompositeNode>(parent);
	if(composite)
	{
		return composite->children;
	}

	auto decorator = dynamic_pointer_cast<DecoratorNode>(parent);
	if(decorator)
	{
		return { decorator->child };
	}

	return vector<shared_ptr<Node>>();
}

void BehaviourTree::Traverse(const shared_ptr<Node>& node, const function<void(const shared_ptr<Node>&)>& visitor)
{
	LOG_INFO();
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

void BehaviourTree::Bind(any* context)
{
	Traverse(rootNode, [&](const shared_ptr<Node>& node)
		{
			node->blackboard = blackboard;
		});
}

Node::~Node()
{
	blackboard.reset();
}

ENodeState Node::Update()
{
	LOG_INFO();
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
			if (node)
			{
				if (node->state == ENodeState::Running)
				{
					node->started = false;
					node->state = ENodeState::Running;
					node->OnStop();
				}
			}
			LOG_INFO();
		});
}

Blackboard::~Blackboard()
{
	tree.reset();
	data.clear();
}
