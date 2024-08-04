// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDTaskNode.h"
#include "BTTask_FindPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UBTTask_FindPatrolPos : public UMDTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindPatrolPos();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnyWhere)
	float FindTargetRadius = 5000.0f;
};
