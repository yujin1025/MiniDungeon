// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackBoardComponent.h"
#include "../../Character/MDCharacter.h"
#include "../MDAIController.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	auto* BlackBoard = GetBlackboardComponent(OwnerComp);
	if (BlackBoard == nullptr)
		return EBTNodeResult::Failed;

	auto* Target = BlackBoard->GetValueAsObject(AMDAIController::TargetObjectKey);
	if (Target == nullptr)
		return EBTNodeResult::Failed;

	auto* TargetCharacter = Cast<AMDCharacter>(Target);
	if (TargetCharacter == nullptr)
		return EBTNodeResult::Failed;

	auto* MyCharacter = GetCharacter(OwnerComp);
	if (MyCharacter == nullptr)
		return EBTNodeResult::Failed;

	MyCharacter->RotateToTarget(TargetCharacter, RotateSpeed);

	return EBTNodeResult::Succeeded;
}
