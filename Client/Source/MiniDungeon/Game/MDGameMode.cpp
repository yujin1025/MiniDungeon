// Fill out your copyright notice in the Description page of Project Settings.


#include "MDGameMode.h"
#include "MDPlayerController.h"
#include "MDGameState.h"
#include "../Widget/MDWidget.h"
#include "Blueprint/UserWidget.h"
#include "MDNetworkManager.h"
#include "InGameLevelScriptActor.h"

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

	CurrentLevelScriptActor = GetWorld()->GetLevelScriptActor();
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

void AMDGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	if (networkManager != nullptr)
	{
		networkManager->HandleRecvPackets();
	}
}

void AMDGameMode::StartPlay()
{
	MD_LOG(LogMDNetwork, Log, TEXT("Super Begin"));
	Super::StartPlay();
	MD_LOG(LogMDNetwork, Log, TEXT("Super End"));

	MD_LOG(LogMDNetwork, Log, TEXT("Override Begin"));

	auto ingameLevelScriptActor = Cast<AInGameLevelScriptActor>(CurrentLevelScriptActor);
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();

	if (networkManager != nullptr)
	{
		for (const auto& playerInfo : networkManager->PlayerInfos)
		{
			if (playerInfo.Value->player_id() == networkManager->PlayerID)
			{
				networkManager->HandleSpawn(*(playerInfo.Value), ingameLevelScriptActor->GetPlayerStarts(), true);
			}
			else
			{
				networkManager->HandleSpawn(*(playerInfo.Value), ingameLevelScriptActor->GetPlayerStarts(), false);
			}
		}

		for(const auto& monsterInfo : networkManager->MonsterInfos)
		{
			uint64 object_id = (monsterInfo.Value)->object_info().object_id();
			FVector spawnLocation = ingameLevelScriptActor->GetEnemySpawns()[object_id % 4]->GetActorLocation();
			networkManager->HandleSpawn((monsterInfo.Value)->object_info(), spawnLocation);
			auto amdGameState = GetGameState<AMDGameState>();
			if (IsValid(amdGameState))
			{
				amdGameState->SetMonsterHealthMap(object_id, monsterInfo.Value->monster_hp());
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
