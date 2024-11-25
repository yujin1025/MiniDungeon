#include "pch.h"
#include "Boss.h"
#include "Player.h"
#include "BehaviourTree.h"
#include "CompositeNode.h"
#include "DecoratorNode.h"
#include "ServiceNode.h"
#include "ActionNode.h"

Boss::Boss()
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

Boss::~Boss()
{
}

void Boss::Init()
{
    blackboard = make_shared<MonsterBlackboard>();
    behaviourTree = make_shared<BehaviourTree>();
    behaviourTree->blackboard = blackboard;
    behaviourTree->owner = dynamic_pointer_cast<Monster>(shared_from_this());
    behaviourTree->rootNode = make_shared<RootNode>(behaviourTree, blackboard);

    auto calcDist = make_shared<DetectionService>(behaviourTree, blackboard);
    behaviourTree->rootNode->child = calcDist;
    calcDist->SetDetectRange(2000.f);

    // 자식 노드 생성
    auto selector1 = make_shared<SelectorNode>(behaviourTree, blackboard);
    calcDist->child = selector1;

    auto hasTargetDeco = make_shared<HasTargetDecorator>(behaviourTree, blackboard);
    selector1->children.push_back(hasTargetDeco);

    auto selector2 = make_shared<SelectorNode>(behaviourTree, blackboard);
    hasTargetDeco->child = selector2;

    auto canAttackDeco = make_shared<CanAttackDecorator>(behaviourTree, blackboard);
    selector2->children.push_back(canAttackDeco);

    // TODO : 몬스터 체력에 따라 공격 선택
    auto checkFirstPhase = make_shared<CheckHealthDecorator>(behaviourTree, blackboard);
    selector2->children.push_back(checkFirstPhase);

    auto checkSecondPhase = make_shared<CheckHealthDecorator>(behaviourTree, blackboard);
    selector2->children.push_back(checkSecondPhase);

    auto checkThirdPhase = make_shared<CheckHealthDecorator>(behaviourTree, blackboard);
    selector2->children.push_back(checkThirdPhase);

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
