// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MDAIController.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API AMDAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMDAIController();

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetObjectKey;
	static const FName DamagedCountKey;
	static const FName DamagedTypeKey;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION()
	void SetCharacterPerception(AMDCharacter* InChar);

	UFUNCTION()
	void OnDamaged(AMDCharacter* Attacker, float Amount);

private:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* BTAsset;

	UPROPERTY(EditAnywhere)
	class UBlackboardData* BBAsset;
};
