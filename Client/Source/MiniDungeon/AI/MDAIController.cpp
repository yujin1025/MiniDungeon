// Fill out your copyright notice in the Description page of Project Settings.


#include "MDAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/MDCharacter.h"
#include "../Component/HealthComponent.h"

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

void AMDAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UBlackboardComponent* blackboardComponent = Blackboard;

	if (UseBlackboard(BBAsset, blackboardComponent))
	{
		blackboardComponent->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		RunBehaviorTree(BTAsset);

		if (!RunBehaviorTree(BTAsset))
		{
			UE_LOG(LogTemp, Log, TEXT("AIController couldn't run"));
		}
	}

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
