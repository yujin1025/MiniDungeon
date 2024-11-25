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

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

public:
	// 이동 명령을 실행하는 함수
	FPathFollowingRequestResult CustomMoveToLocation(const FVector& Destination, float AcceptanceRadius = 50.0f);

	FPathFollowingRequestResult CustomMoveToActor(AActor* DestinationActor, float AcceptanceRadius = 50.0f);

	void OnMoveCompletedHandler(FAIRequestID RequestID, const FPathFollowingResult& Result);

protected:
	virtual void BeginPlay() override;
};
