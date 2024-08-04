// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackBoardComponent.h"
#include "../../Character/MDCharacter.h"
#include "../MDAIController.h"

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto* MyCharacter = GetCharacter(OwnerComp);
	if (MyCharacter == nullptr)
		return false;

	auto* BlackBoard = GetBlackboardComponent(OwnerComp);
	if (BlackBoard == nullptr)
		return false;

	auto TargetObject = BlackBoard->GetValueAsObject(AMDAIController::TargetObjectKey);
	if (TargetObject == nullptr)
		return false;

	auto* TargetCharacter = Cast<AMDCharacter>(TargetObject);
	if (TargetCharacter == nullptr)
		return false;

	float distance = TargetCharacter->GetDistanceTo(MyCharacter);
	return distance <= AttackDistance;
}
