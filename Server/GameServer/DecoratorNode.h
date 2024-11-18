#pragma once
#include "BehaviourTree.h"

class DecoratorNode : public Node
{
public:
	DecoratorNode() = default;
	virtual ~DecoratorNode() = default;

public:
	shared_ptr<Node> child;
};

