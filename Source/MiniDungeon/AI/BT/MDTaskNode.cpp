// Fill out your copyright notice in the Description page of Project Settings.


#include "MDTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "../MDAIController.h"
#include "AIController.h"


UNavigationSystemV1* UMDTaskNode::GetNavigationSystem(UBehaviorTreeComponent& OwnerComp)
{
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn == nullptr)
		return nullptr;

	return UNavigationSystemV1::GetNavigationSystem(Pawn->GetWorld());
}

AMDAIController* UMDTaskNode::GetAIController(UBehaviorTreeComponent& OwnerComp)
{
	return Cast<AMDAIController>(OwnerComp.GetAIOwner());
}

UBlackboardComponent* UMDTaskNode::GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp)
{
	return OwnerComp.GetBlackboardComponent();
}
