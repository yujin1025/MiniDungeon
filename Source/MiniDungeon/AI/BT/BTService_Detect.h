// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDService.h"
#include "BTService_Detect.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API UBTService_Detect : public UMDService
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool TryGetOverlapResult(AMDCharacter* Owner, TArray<FOverlapResult>& OverlapResults);
	bool TryGetOverlapTarget(AMDCharacter* Owner, OUT AMDCharacter*& FoundTarget);
	void SetDetectedTarget(UBehaviorTreeComponent& OwnerComp, AMDCharacter* TargetCharacter);

private:
	UPROPERTY(EditAnywhere)
	float DetectRadius = 600.0f;
};
