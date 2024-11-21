#include "pch.h"
#include "DecoratorNode.h"
#include "Monster.h"
#include "Player.h"
#include "CompositeNode.h"

DecoratorNode::~DecoratorNode()
{
    child.reset();
}

void RepeaterNode::OnStart()
{
}

void RepeaterNode::OnStop()
{
}

ENodeState RepeaterNode::OnUpdate()
{
    LOG_INFO();

    if (child != nullptr)
    {
        switch (child->Update())
        {
        case ENodeState::Running:
            break;
        case ENodeState::Failure:
            if (restartOnFailure)
            {
                return ENodeState::Running;
            }
            else
            {
                return ENodeState::Failure;
            }
        case ENodeState::Success:
            if (restartOnSuccess)
            {
                return ENodeState::Running;
            }
            else
            {
                return ENodeState::Success;
            }
        }
        return ENodeState::Running;
    }
    
    return ENodeState::Failure;
}

void RootNode::OnStart()
{
}

void RootNode::OnStop()
{
}

ENodeState RootNode::OnUpdate()
{
    LOG_INFO();
    if(child != nullptr)
	{
		return child->Update();
	}

    return ENodeState::Failure;
}

void HasTargetDecorator::OnStart()
{
}

void HasTargetDecorator::OnStop()
{
}

ENodeState HasTargetDecorator::OnUpdate()
{
    LOG_INFO();
    auto bt = tree.lock();
    if (bt == nullptr)
        return ENodeState::Failure;

    auto ownerMonster = bt->owner.lock();
    if (ownerMonster == nullptr)
        return ENodeState::Failure;

    auto target = ownerMonster->TargetPlayer.load();
    if (target == nullptr)
    {
        return ENodeState::Failure;
    }
    else
    {
        Vector3 moveToPosition = any_cast<Vector3>(blackboard->GetData(EBlackboardKey::Position));
        moveToPosition.x = target->GetPosInfo().x();
        moveToPosition.y = target->GetPosInfo().y();
        moveToPosition.z = target->GetPosInfo().z();
        if(child == nullptr)
			return ENodeState::Failure;

        return child->Update();
    }
}

void NoTargetDecorator::OnStart()
{
}

void NoTargetDecorator::OnStop()
{
}

ENodeState NoTargetDecorator::OnUpdate()
{
    LOG_INFO();

    auto bt = tree.lock();
    if (bt == nullptr)
        return ENodeState::Failure;

    auto ownerMonster = bt->owner.lock();
    if (ownerMonster == nullptr)
        return ENodeState::Failure;

    auto target = ownerMonster->TargetPlayer.load();
    if (target == nullptr)
    {
        if(child == nullptr)
            return ENodeState::Failure;

        return child->Update();
    }
    else
    {
        return ENodeState::Failure;
    }
}

void CanAttackDecorator::OnStart()
{

}

void CanAttackDecorator::OnStop()
{

}

ENodeState CanAttackDecorator::OnUpdate()
{
    LOG_INFO();
    auto bt = tree.lock();
    if (bt == nullptr)
        return ENodeState::Failure;

    auto ownerMonster = bt->owner.lock();
    if (ownerMonster == nullptr)
        return ENodeState::Failure;

    auto target = ownerMonster->TargetPlayer.load();
    if (target == nullptr)
        return ENodeState::Failure;

    // 공격 범위 확인
    float distance = ownerMonster->DistanceTo(target->GetPosInfo());
    if (distance <= AttackRange)
    {
        // 조건 충족: 하위 노드 실행
        if(child == nullptr)
			return ENodeState::Failure;

        return child->Update();
    }

    return ENodeState::Failure; // 조건 불충족
}

void CanNotAttackDecorator::OnStart()
{
}

void CanNotAttackDecorator::OnStop()
{
}

ENodeState CanNotAttackDecorator::OnUpdate()
{
    LOG_INFO();
    auto bt = tree.lock();
    if (bt == nullptr)
        return ENodeState::Failure;

    auto ownerMonster = bt->owner.lock();
    if (ownerMonster == nullptr)
        return ENodeState::Failure;

    auto target = ownerMonster->TargetPlayer.load();
    if (target == nullptr)
        return ENodeState::Failure;

    // 공격 범위 확인
    float distance = ownerMonster->DistanceTo(target->GetPosInfo());
    if (distance > AttackRange)
    {
        if (blackboard == nullptr)
        {
            return ENodeState::Failure;
        }
        blackboard->SetData(EBlackboardKey::Position, Vector3(target->GetPosInfo().x(), target->GetPosInfo().y(), target->GetPosInfo().z()));

        if (child == nullptr)
        {
            return ENodeState::Failure;
        }

        // 조건 충족: 하위 노드 실행
        return child->Update();
    }

    return ENodeState::Failure; // 조건 불충족
}
