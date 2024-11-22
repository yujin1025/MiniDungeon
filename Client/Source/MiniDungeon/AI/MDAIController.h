// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
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
	void SetCharacterPerception(class AMDCharacter* InChar);

	UFUNCTION()
	void OnDamaged(class AMDCharacter* Attacker, float Amount);

	void SetBlackboardValues(bool isFindPlayer, class APlayableCharacter* target, FVector playerLocation, float speed, float calcDist);

public:
	// 이동 명령을 실행하는 함수
	FPathFollowingRequestResult CustomMoveTo(const FVector& Destination, float AcceptanceRadius = 50.0f);

	void OnMoveCompletedHandler(FAIRequestID RequestID, const FPathFollowingResult& Result);

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* BTAsset;

	UPROPERTY(EditAnywhere)
	class UBlackboardData* BBAsset;
};
