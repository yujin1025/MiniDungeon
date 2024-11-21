// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "../MDAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include <MDNetworkManager.h>

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	auto* NavSystem = GetNavigationSystem(OwnerComp);
	if (NavSystem == nullptr)
		return EBTNodeResult::Failed;

	auto* BlackBoard = GetBlackboardComponent(OwnerComp);
	if (BlackBoard == nullptr)
		return EBTNodeResult::Failed;

	FNavLocation NextPatrol;
	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, FindTargetRadius, NextPatrol) == false)
		return EBTNodeResult::Failed;

	BlackBoard->SetValueAsVector(AMDAIController::PatrolPosKey, NextPatrol.Location);

	auto networdManager = GetAIController(OwnerComp)->GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	if(IsValid(networdManager))
	{
		// TODO : Send NextPatrol.Location Info to Server
	}

	return EBTNodeResult::Succeeded;
}
