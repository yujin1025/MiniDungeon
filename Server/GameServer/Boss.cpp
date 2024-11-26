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
    calcDist->SetDetectRange(1500.f);

    // 자식 노드 생성
    auto selector1 = make_shared<SelectorNode>(behaviourTree, blackboard);
    calcDist->child = selector1;

    auto hasTargetDeco = make_shared<HasTargetDecorator>(behaviourTree, blackboard);
    selector1->children.push_back(hasTargetDeco);

    auto selector2 = make_shared<SelectorNode>(behaviourTree, blackboard);
    hasTargetDeco->child = selector2;

    auto canAttackDeco = make_shared<CanAttackDecorator>(behaviourTree, blackboard);
    selector2->children.push_back(canAttackDeco);
    canAttackDeco->SetAttackRange(500.f);

    auto selector3 = make_shared<SelectorNode>(behaviourTree, blackboard);
    canAttackDeco->child = selector3;

    // TODO : 몬스터 체력에 따라 공격 선택
    auto checkFirstPhase = make_shared<CheckHealthDecorator>(behaviourTree, blackboard);
    selector3->children.push_back(checkFirstPhase);
    checkFirstPhase->SetHealthRange(250.f, 400.f);

    auto firstAttackNode = make_shared<AttackNode>(behaviourTree, blackboard);
    checkFirstPhase->child = firstAttackNode;
    firstAttackNode->SetIsRangeAttack(true);
    firstAttackNode->SetAttackType(0);
    firstAttackNode->SetDamage(30.f);
    firstAttackNode->SetHitAngle(60.f);
    firstAttackNode->SetHitRange(300.f);
    firstAttackNode->SetDuration(2.f);

    auto checkSecondPhase = make_shared<CheckHealthDecorator>(behaviourTree, blackboard);
    selector3->children.push_back(checkSecondPhase);
    checkSecondPhase->SetHealthRange(100.f, 250.f);

    auto secondAttackNode = make_shared<AttackNode>(behaviourTree, blackboard);
    checkSecondPhase->child = secondAttackNode;
    firstAttackNode->SetIsRangeAttack(true);
    secondAttackNode->SetAttackType(1);
    secondAttackNode->SetDamage(50.f);
    secondAttackNode->SetHitAngle(70.f);
    secondAttackNode->SetHitRange(400.f);
    secondAttackNode->SetDuration(1.5f);

    auto checkThirdPhase = make_shared<CheckHealthDecorator>(behaviourTree, blackboard);
    selector3->children.push_back(checkThirdPhase);
    checkThirdPhase->SetHealthRange(0.f, 100.f);

    auto thirdAttackNode = make_shared<AttackNode>(behaviourTree, blackboard);
    checkThirdPhase->child = thirdAttackNode;
    firstAttackNode->SetIsRangeAttack(true);
    thirdAttackNode->SetAttackType(2);
    thirdAttackNode->SetDamage(70.f);
    thirdAttackNode->SetHitAngle(80.f);
    thirdAttackNode->SetHitRange(500.f);
    secondAttackNode->SetDuration(1.f);

    // 공격 못 하면 이동
    auto canNotAttackDeco = make_shared<CanNotAttackDecorator>(behaviourTree, blackboard);
    selector2->children.push_back(canNotAttackDeco);

    auto moveToPlayerNode = make_shared<MoveToPlayer>(behaviourTree, blackboard);
    canNotAttackDeco->child = moveToPlayerNode;

    // 타겟이 없을 때
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

    _canBTRun = true;
}
