// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MDTaskNode.generated.h"

class UNavigationSystemV1;
class AMDAIController;
class UBlackboardComponent;
class UBehaviorTreeComponent;
class AAIController;
/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UMDTaskNode : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	UNavigationSystemV1* GetNavigationSystem(UBehaviorTreeComponent& OwnerComp);
	AMDAIController* GetAIController(UBehaviorTreeComponent& OwnerComp);
	UBlackboardComponent* GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp);
	
};
