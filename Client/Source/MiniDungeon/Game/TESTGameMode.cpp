// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TESTGameMode.h"
#include <Character/Aurora.h>
#include "MDGameInstance.h"
#include <Kismet/GameplayStatics.h>
#include "MDPlayerController.h"
#include "MiniDungeon.h"
#include "TESTPlayerController.h"

ATESTGameMode::ATESTGameMode()
{
}

void ATESTGameMode::BeginPlay()
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::BeginPlay();
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

void ATESTGameMode::PostInitializeComponents()
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::PostInitializeComponents();
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

void ATESTGameMode::PostLogin(APlayerController* NewPlayer)
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::PostLogin(NewPlayer);
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

void ATESTGameMode::StartPlay()
{		
	MD_LOG(LogMDNetwork, Log, TEXT("Super Begin"));
	Super::StartPlay();
	MD_LOG(LogMDNetwork, Log, TEXT("Super End"));

	MD_LOG(LogMDNetwork, Log, TEXT("Child Begin"));
	ATESTPlayerController* pc = Cast<ATESTPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (!IsValid(pc))
	{
		MD_LOG(LogMDNetwork, Log, TEXT("TESTPC IS NULL"));
		return;
	}

	APlayableCharacter* player = Cast<APlayableCharacter>(pc->GetPawn());

	if (!IsValid(player))
	{
		FVector location = FVector(940.0f, 400.0f, 120.0f);
		player = Cast<APlayableCharacter>(GetWorld()->SpawnActor(Cast<UMDGameInstance>(GetGameInstance())->AuroraClass, &location));
		if (IsValid(pc))
		{
			MD_LOG(LogMDNetwork, Log, TEXT("PC POSSESS TO PLAYER BEGIN"));
			pc->OnPossess(player);
			MD_LOG(LogMDNetwork, Log, TEXT("PC POSSESS TO PLAYER END"));
		}
	}

	MD_LOG(LogMDNetwork, Log, TEXT("Child End"));
}

void ATESTGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
}
