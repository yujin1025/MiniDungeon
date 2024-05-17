// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MDService.generated.h"

class AMDCharacter;
/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UMDService : public UBTService
{
	GENERATED_BODY()
	
protected:
	AMDCharacter* GetCharacter(UBehaviorTreeComponent& OwnerComp);
	UBlackboardComponent* GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp);

public:
	static AMDCharacter* GetCharacter(const FOverlapResult& OverlapResult);
};
