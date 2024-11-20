#include "pch.h"
#include "DecoratorNode.h"
#include "Monster.h"
#include "Player.h"
#include "CompositeNode.h"

DecoratorNode::~DecoratorNode()
{
    child.reset();
}

void DecoratorNode::OnStart()
{
}

void DecoratorNode::OnStop()
{
}

ENodeState DecoratorNode::OnUpdate()
{

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

RootNode::RootNode()
{
    restartOnSuccess = true;
    restartOnFailure = true;
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
    return child->Update();
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
    auto ownerMonster = tree.lock()->owner.lock();
    if (!ownerMonster)
        return ENodeState::Failure;

    auto target = ownerMonster->TargetPlayer.load();
    if (!target)
    {
        return ENodeState::Failure;
    }
    else
    {
        Vector3 moveToPosition = any_cast<Vector3>(blackboard->data[EBlackboardKey::Position]);
        moveToPosition.x = target->GetPosInfo().x();
        moveToPosition.y = target->GetPosInfo().y();
        moveToPosition.z = target->GetPosInfo().z();
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
    auto ownerMonster = tree.lock()->owner.lock();
    if (!ownerMonster)
        return ENodeState::Failure;

    auto target = ownerMonster->TargetPlayer.load();
    if (!target)
    {
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
    auto ownerMonster = tree.lock()->owner.lock();
    if (!ownerMonster)
        return ENodeState::Failure;

    auto target = ownerMonster->TargetPlayer.load();
    if (!target)
        return ENodeState::Failure;

    // 공격 범위 확인
    float distance = ownerMonster->DistanceTo(target->GetPosInfo());
    if (distance <= AttackRange)
    {
        // 조건 충족: 하위 노드 실행
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
    auto ownerMonster = tree.lock()->owner.lock();
    if (!ownerMonster)
        return ENodeState::Failure;

    auto target = ownerMonster->TargetPlayer.load();
    if (!target)
        return ENodeState::Failure;

    // 공격 범위 확인
    float distance = ownerMonster->DistanceTo(target->GetPosInfo());
    if (distance > AttackRange)
    {
        // 조건 충족: 하위 노드 실행
        return child->Update();
    }

    return ENodeState::Failure; // 조건 불충족
}
