// Fill out your copyright notice in the Description page of Project Settings.


#include "MDDecorator.h"
#include "../../Character/MDCharacter.h"
#include "AIController.h"


AMDCharacter* UMDDecorator::GetCharacter(UBehaviorTreeComponent& OwnerComp) const
{
	auto* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn == nullptr)
		return nullptr;

	return Cast<AMDCharacter>(Pawn);
}

UBlackboardComponent* UMDDecorator::GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp) const
{
	return OwnerComp.GetBlackboardComponent();
}
