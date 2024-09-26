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
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::BeginPlay();
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
	ingameWindowWidget = CreateWidget<UMDWidget>(GetWorld(), ingameWindowWidgetClass);
	if (ingameWindowWidget != nullptr)
	{
		ingameWindowWidget->AddToViewport();
	}
}

void AMDGameMode::PostInitializeComponents()
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::PostInitializeComponents();
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
	MyGameState = Cast<AMDGameState>(GameState);
}

void AMDGameMode::PostLogin(APlayerController* NewPlayer)
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::PostLogin(NewPlayer);
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
	OnPostLogin(Cast<AMDPlayerController>(NewPlayer));
}

void AMDGameMode::StartPlay()
{
	MD_LOG(LogMDNetwork, Log, TEXT("Super Begin"));
	Super::StartPlay();
	MD_LOG(LogMDNetwork, Log, TEXT("Super End"));

	MD_LOG(LogMDNetwork, Log, TEXT("Override Begin"));
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
	MD_LOG(LogMDNetwork, Log, TEXT("Override End"));
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
