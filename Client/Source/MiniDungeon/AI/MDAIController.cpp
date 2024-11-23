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

const FName AMDAIController::HomePosKey(TEXT("HomePos"));
const FName AMDAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AMDAIController::TargetObjectKey(TEXT("TargetObject"));
const FName AMDAIController::DamagedCountKey(TEXT("DamagedCount"));
const FName AMDAIController::DamagedTypeKey(TEXT("DamagedType"));

AMDAIController::AMDAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Assets/AI/BB_Khaimera.BB_Khaimera"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Assets/AI/BT_Khaimera.BT_Khaimera"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void AMDAIController::BeginPlay()
{
	Super::BeginPlay();
	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AMDAIController::OnMoveCompletedHandler);
}

void AMDAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//UBlackboardComponent* blackboardComponent = Blackboard;

	//if (UseBlackboard(BBAsset, blackboardComponent))
	//{
	//	blackboardComponent->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
	//	//RunBehaviorTree(BTAsset);

	//	if (!RunBehaviorTree(BTAsset))
	//	{
	//		UE_LOG(LogTemp, Log, TEXT("AIController couldn't run"));
	//	}
	//}

	auto* InChar = Cast<AMDCharacter>(InPawn);
	if (InChar == nullptr)
		return;

	SetCharacterPerception(InChar);
}

void AMDAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AMDAIController::SetCharacterPerception(AMDCharacter* InChar)
{
	InChar->HealthComponent->OnDamaged.AddUObject(this, &AMDAIController::OnDamaged);
}

void AMDAIController::OnDamaged(AMDCharacter* Attacker, float Amount)
{
	UBlackboardComponent* blackboardComponent = Blackboard;
	if (blackboardComponent == nullptr)
		return;

	int DamagedCount = blackboardComponent->GetValueAsInt(DamagedCountKey);
	blackboardComponent->SetValueAsInt(DamagedCountKey, DamagedCount + 1);
}

void AMDAIController::SetBlackboardValues(bool isFindPlayer, APlayableCharacter* target, FVector playerLocation, float speed, float calcDist)
{
	//UBlackboardComponent* blackboardComponent = Blackboard;

	//if (blackboardComponent)
	//{
	//	blackboardComponent->SetValueAsBool(TEXT("IsFindPlayer"), isFindPlayer);
	//	if (target)
	//		blackboardComponent->SetValueAsObject(TEXT("TargetObject"), target);
	//	blackboardComponent->SetValueAsVector(TEXT("PlayerPos"), playerLocation);
	//	blackboardComponent->SetValueAsFloat(TEXT("Speed"), speed);
	//	blackboardComponent->SetValueAsFloat(TEXT("CalcDist"), calcDist);
	//}
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