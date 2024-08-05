// Fill out your copyright notice in the Description page of Project Settings.


#include "MDGameMode.h"
#include "MDPlayerController.h"
#include "MDGameState.h"
#include "../Widget/MDWidget.h"
#include "Blueprint/UserWidget.h"

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
