// Fill out your copyright notice in the Description page of Project Settings.


#include "MDService.h"
#include "AIController.h"
#include "../../Character/MDCharacter.h"

AMDCharacter* UMDService::GetCharacter(UBehaviorTreeComponent& OwnerComp)
{
	auto* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn == nullptr)
		return nullptr;

	return Cast<AMDCharacter>(Pawn);
}

UBlackboardComponent* UMDService::GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp)
{
	return OwnerComp.GetBlackboardComponent();
}

AMDCharacter* UMDService::GetCharacter(const FOverlapResult& OverlapResult)
{
	auto* Actor = OverlapResult.GetActor();
	if (Actor == nullptr)
		return nullptr;

	return Cast<AMDCharacter>(Actor);
}
