// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "MDDecorator.generated.h"

class AMDCharacter;
class UBehaviorTreeComponent;
class UBlackboardComponent;
/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UMDDecorator : public UBTDecorator
{
	GENERATED_BODY()
	
protected:
	AMDCharacter* GetCharacter(UBehaviorTreeComponent& OwnerComp) const;
	UBlackboardComponent* GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp) const;
};
