// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDDecorator.h"
#include "BTDecorator_IsLowHealth.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UBTDecorator_IsLowHealth : public UMDDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_IsLowHealth();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
	UPROPERTY(EditAnywhere)
	int MaxHealth;

	UPROPERTY(EditAnywhere)
	int MinHealth;

};
