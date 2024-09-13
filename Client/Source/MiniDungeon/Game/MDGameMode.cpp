// Fill out your copyright notice in the Description page of Project Settings.


#include "MDGameMode.h"
#include "MDPlayerController.h"
#include "MDGameState.h"
#include "../Widget/MDWidget.h"
#include "Blueprint/UserWidget.h"
#include "MDNetworkManager.h"

AMDGameMode::AMDGameMode()
{
	CharacterStatTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Assets/Data/CharacterStat"));
}

void AMDGameMode::BeginPlay()
{
	Super::BeginPlay();

	ingameWindowWidget = CreateWidget<UMDWidget>(GetWorld(), ingameWindowWidgetClass);
	if (ingameWindowWidget != nullptr)
	{
		ingameWindowWidget->AddToViewport();
	}
}

void AMDGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MyGameState = Cast<AMDGameState>(GameState);
}

void AMDGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	OnPostLogin(Cast<AMDPlayerController>(NewPlayer));
}

void AMDGameMode::StartPlay()
{
	Super::StartPlay();

	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	if (networkManager != nullptr)
	{
		for (const auto& playerInfo : networkManager->PlayerInfos)
		{
			if (playerInfo.Value->player_id() == networkManager->PlayerID)
			{
				networkManager->HandleSpawn(*(playerInfo.Value), true);
			}
			else
			{
				networkManager->HandleSpawn(*(playerInfo.Value), false);
			}

		}
	}
}


void AMDGameMode::OnPostLogin(AController* NewPlayer)
{
	auto newPlayer = Cast<AMDPlayerController>(NewPlayer);
	if (IsValid(newPlayer))
	{
		MyPlayerState = newPlayer->GetState();
	}
}

FCharacterStatData* AMDGameMode::GetCharacterStat(ECharacterType type)
{
	int IntType = (int)type;
	FName StringType = *FString::FromInt(IntType);

	return CharacterStatTable->FindRow<FCharacterStatData>(StringType, TEXT(""));
}
