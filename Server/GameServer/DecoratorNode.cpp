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
    auto bt = tree.lock();
    if (bt == nullptr)
    {
        LOG("HasTargetDecorator bt is nullptr");
        return ENodeState::Failure;
    }

    auto ownerMonster = bt->owner.lock();
    if (ownerMonster == nullptr)
    {
        LOG("HasTargetDecorator ownerMonster is nullptr");
        return ENodeState::Failure;
    }


    auto target = ownerMonster->TargetPlayer.load();
    if (target == nullptr)
    {
        blackboard->SetData(EBlackboardKey::Target, 0);
        LOG("HasTargetDecorator target is nullptr");
        return ENodeState::Failure;
    }
    else
    {
        uint64 target_id = target->GetObjectInfo().object_id();
        blackboard->SetData(EBlackboardKey::Target, target_id);

        if (child == nullptr)
        {
            LOG("HasTargetDecorator child is nullptr");
			return ENodeState::Failure;
        }

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
    auto bt = tree.lock();
    if (bt == nullptr)
    {
        LOG("NoTargetDecorator bt is nullptr");
        return ENodeState::Failure;
    }

    auto ownerMonster = bt->owner.lock();
    if (ownerMonster == nullptr)
    {
		LOG("NoTargetDecorator ownerMonster is nullptr");
		return ENodeState::Failure;
    }

    auto target = ownerMonster->TargetPlayer.load();
    if (target == nullptr)
    {
        blackboard->SetData(EBlackboardKey::Target, 0);
        if (child == nullptr)
        {
            LOG("NoTargetDecorator child is nullptr");
            return ENodeState::Failure;
        }
        return child->Update();
    }
    else
    {
        LOG("monster Has Target");
        uint64 target_id = target->GetObjectInfo().object_id();
        blackboard->SetData(EBlackboardKey::Target, target_id);
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
    auto bt = tree.lock();
    if (bt == nullptr)
    {
        LOG("CanAttackDecorator bt is nullptr");
        return ENodeState::Failure;
    }


    auto ownerMonster = bt->owner.lock();
    if (ownerMonster == nullptr)
    {
        LOG("CanAttackDecorator ownerMonster is nullptr");
        return ENodeState::Failure;
    }


    auto target = ownerMonster->TargetPlayer.load();
    if (target == nullptr)
    {
        blackboard->SetData(EBlackboardKey::Target, 0);
        LOG("CanAttackDecorator target is nullptr");
        return ENodeState::Failure;
    }

    // 공격 범위 확인
    auto targetPosInfo = target->GetPosInfo();
    blackboard->SetData(EBlackboardKey::Target, targetPosInfo.object_id());

    float distance = ownerMonster->DistanceTo(targetPosInfo);
    if (distance <= AttackRange)
    {
        // 조건 충족: 하위 노드 실행
        if (child == nullptr)
        {
            LOG("CanAttackDecorator child is nullptr");
			return ENodeState::Failure;
        }
        return child->Update();
    }

    LOG("Target is out of range");
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
    auto bt = tree.lock();
    if (bt == nullptr)
    {
        LOG("CanNotAttackDecorator bt is nullptr");
        return ENodeState::Failure;
    }


    auto ownerMonster = bt->owner.lock();
    if (ownerMonster == nullptr)
    {
        LOG("CanNotAttackDecorator ownerMonster is nullptr");
        return ENodeState::Failure;
    }


    auto target = ownerMonster->TargetPlayer.load();
    if (target == nullptr)
    {
        blackboard->SetData(EBlackboardKey::Target, 0);
        LOG("CanNotAttackDecorator target is nullptr");
        return ENodeState::Failure;
    }

    auto targetPosInfo = target->GetPosInfo();
    blackboard->SetData(EBlackboardKey::Target, targetPosInfo.object_id());
    // 공격 범위 확인
    float distance = ownerMonster->DistanceTo(targetPosInfo);
    if (distance > AttackRange)
    {
        if (blackboard == nullptr)
        {
            LOG("CanNotAttackDecorator blackboard is nullptr");
            return ENodeState::Failure;
        }

        if (child == nullptr)
        {
            LOG("CanNotAttackDecorator child is nullptr");
            return ENodeState::Failure;
        }

        // 조건 충족: 하위 노드 실행
        return child->Update();
    }

    LOG("Target is in range");
    return ENodeState::Failure; // 조건 불충족
}

void CheckHealthDecorator::OnStart()
{
}

void CheckHealthDecorator::OnStop()
{
}

ENodeState CheckHealthDecorator::OnUpdate()
{
    auto bt = tree.lock();
    if (bt == nullptr)
    {
        LOG("HasTargetDecorator bt is nullptr");
        return ENodeState::Failure;
    }

    auto ownerMonster = bt->owner.lock();
    if (ownerMonster == nullptr)
    {
        LOG("HasTargetDecorator ownerMonster is nullptr");
        return ENodeState::Failure;
    }

    auto target = ownerMonster->TargetPlayer.load();
    if (target == nullptr)
    {
        blackboard->SetData(EBlackboardKey::Target, 0);
        LOG("HasTargetDecorator target is nullptr");
        return ENodeState::Failure;
    }

    auto targetPosInfo = target->GetPosInfo();
    blackboard->SetData(EBlackboardKey::Target, targetPosInfo.object_id());

    // 체력 확인
    float currentHealth = ownerMonster->GetHp();

    if(currentHealth <= 0)
	{
		return ENodeState::Failure;
	}

    if (minHealth <= currentHealth && currentHealth <= maxHealth)
    {
        if (blackboard == nullptr)
        {
            LOG("CheckHealthDecorator blackboard is nullptr");
            return ENodeState::Failure;
        }

        if (child == nullptr)
        {
            LOG("CheckHealthDecorator child is nullptr");
            return ENodeState::Failure;
        }

        // 조건 충족: 하위 노드 실행
        return child->Update();
    }

    LOG("Health is out of range");
    return ENodeState::Failure; // 조건 불충족
}
