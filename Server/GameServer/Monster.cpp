#include "pch.h"
#include "Monster.h"
#include "Player.h"
#include "BehaviourTree.h"
#include "CompositeNode.h"
#include "DecoratorNode.h"
#include "ServiceNode.h"
#include "ActionNode.h"

Monster::Monster()
{
	monsterInfo = new Protocol::MonsterInfo();
    Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
    objectInfo->set_creature_type(Protocol::CreatureType::CREATURE_TYPE_MONSTER);
    obj_info->CopyFrom(*objectInfo);

    monsterInfo->set_allocated_object_info(obj_info);
    monsterInfo->set_monster_hp(MaxHp);

    _hp = MaxHp;
    CurrentHp = MaxHp;
}

Monster::~Monster()
{
	delete monsterInfo;
    monsterInfo = nullptr;
}

void Monster::Init()
{
    blackboard = make_shared<MonsterBlackboard>();
    behaviourTree = make_shared<BehaviourTree>();
    behaviourTree->blackboard = blackboard;
    behaviourTree->owner = dynamic_pointer_cast<Monster>(shared_from_this());
    behaviourTree->rootNode = make_shared<RootNode>(behaviourTree, blackboard);

    auto calcDist = make_shared<DetectionService>(behaviourTree, blackboard);
    behaviourTree->rootNode->child = calcDist;

    // 자식 노드 생성
    auto selector1 = make_shared<SelectorNode>(behaviourTree, blackboard);
    calcDist->child = selector1;

    auto hasTargetDeco = make_shared<HasTargetDecorator>(behaviourTree, blackboard);
    selector1->children.push_back(hasTargetDeco);

    auto selector2 = make_shared<SelectorNode>(behaviourTree, blackboard);
    hasTargetDeco->child = selector2;

    auto canAttackDeco = make_shared<CanAttackDecorator>(behaviourTree, blackboard);
    selector2->children.push_back(canAttackDeco);

    auto attackNode = make_shared<AttackNode>(behaviourTree, blackboard);
    canAttackDeco->child = attackNode;

    auto canNotAttackDeco = make_shared<CanNotAttackDecorator>(behaviourTree, blackboard);
    selector2->children.push_back(canNotAttackDeco);

    auto moveToPlayerNode = make_shared<MoveToPlayer>(behaviourTree, blackboard);
    canNotAttackDeco->child = moveToPlayerNode;

    auto noTaragetDeco = make_shared<NoTargetDecorator>(behaviourTree, blackboard);
    selector1->children.push_back(noTaragetDeco);

    auto sequencer1 = make_shared<SequencerNode>(behaviourTree, blackboard);
    noTaragetDeco->child = sequencer1;

    auto waitNode = make_shared<WaitNode>(behaviourTree, blackboard);
    sequencer1->children.push_back(waitNode);

    auto randomPositionNode = make_shared<RandomPosition>(behaviourTree, blackboard);
    sequencer1->children.push_back(randomPositionNode);

    auto moveToPositionNode = make_shared<MoveToPosition>(behaviourTree, blackboard);
    sequencer1->children.push_back(moveToPositionNode);
}

float Monster::DistanceTo(const Protocol::PosInfo& targetPos)
{
    // 보스 위치 (posInfo)와 타겟 플레이어 위치 (targetPos) 간의 거리 계산
    float dx = GetPosInfo().x() - targetPos.x();
    float dy = GetPosInfo().y() - targetPos.y();

    return sqrt(dx * dx + dy * dy);
}

void Monster::SetObjectInfo(const Protocol::ObjectInfo& obj_Info)
{
    WRITE_LOCK;
	Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
	obj_info->CopyFrom(obj_Info);

	monsterInfo->set_allocated_object_info(obj_info);
    objectInfo->CopyFrom(monsterInfo->object_info());
}

void Monster::SetPosInfo(const Protocol::PosInfo& pos_Info)
{
    WRITE_LOCK;
    Protocol::PosInfo* pos_info = new Protocol::PosInfo();
	pos_info->CopyFrom(pos_Info);

	Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
	obj_info->CopyFrom(*objectInfo);
	obj_info->set_allocated_pos_info(pos_info);

	monsterInfo->set_allocated_object_info(obj_info);
    objectInfo->CopyFrom(monsterInfo->object_info());
}

void Monster::SetMovementState(Protocol::MoveState state)
{
    WRITE_LOCK;
	Protocol::PosInfo* pos_info = new Protocol::PosInfo();
	pos_info->CopyFrom(objectInfo->pos_info());
	pos_info->set_state(state);

	Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
	obj_info->CopyFrom(*objectInfo);
	obj_info->set_allocated_pos_info(pos_info);

	monsterInfo->set_allocated_object_info(obj_info);
	objectInfo->CopyFrom(monsterInfo->object_info());
}

void Monster::UpdateBehaviourTree()
{
    if (behaviourTree)
    {
        behaviourTree->Update();
    }
}
