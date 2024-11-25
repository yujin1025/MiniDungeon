// Fill out your copyright notice in the Description page of Project Settings.


#include "MDAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Component/HealthComponent.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Character/PlayableCharacter.h"
#include <Character/NonPlayableCharacter.h>
#include <MDNetworkManager.h>

AMDAIController::AMDAIController()
{
}

void AMDAIController::BeginPlay()
{
	Super::BeginPlay();
	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AMDAIController::OnMoveCompletedHandler);
}

void AMDAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AMDAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

FPathFollowingRequestResult AMDAIController::CustomMoveToLocation(const FVector& Destination, float AcceptanceRadius)
{
	auto ower = Cast<ANonPlayableCharacter>(GetPawn());
	if (ower == nullptr)
		return FPathFollowingRequestResult();

	ower->SetMoveState(Protocol::MOVE_STATE_RUN);

	FAIMoveRequest moveRequest;
	moveRequest.SetGoalLocation(Destination);
	moveRequest.SetAcceptanceRadius(AcceptanceRadius);
	

	FNavPathSharedPtr Path;
	return MoveTo(moveRequest, &Path); // 기본 MoveTo 함수 호출
}

FPathFollowingRequestResult AMDAIController::CustomMoveToActor(AActor* DestinationActor, float AcceptanceRadius)
{
	auto ower = Cast<ANonPlayableCharacter>(GetPawn());
	if (ower == nullptr)
		return FPathFollowingRequestResult();

	ower->SetMoveState(Protocol::MOVE_STATE_RUN);

	FAIMoveRequest moveRequest;
	moveRequest.SetGoalActor(DestinationActor);
	moveRequest.SetAcceptanceRadius(AcceptanceRadius);

	FNavPathSharedPtr Path;

	return MoveTo(moveRequest, &Path); // 기본 MoveToActor 함수 호출
}

void AMDAIController::OnMoveCompletedHandler(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	auto ower = Cast<ANonPlayableCharacter>(GetPawn());
	if (ower == nullptr)
		return;

	ower->SetMoveState(Protocol::MOVE_STATE_IDLE);

	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	if (networkManager)
	{
		Protocol::CTS_MOVE MovePkt;
		Protocol::PosInfo* Info = new Protocol::PosInfo();
		Info->set_object_id(ower->GetObjectID());
		Info->set_x(ower->GetActorLocation().X);
		Info->set_y(ower->GetActorLocation().Y);
		Info->set_z(ower->GetActorLocation().Z);
		Info->set_yaw(ower->GetControlRotation().Yaw);
		Info->set_state(ower->GetMoveState());
		MovePkt.set_allocated_info(Info);

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(MovePkt);
		if (IsValid(networkManager))
		{
			networkManager->SendPacket(sendBuffer);
		}
	}
}