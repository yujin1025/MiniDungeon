// Fill out your copyright notice in the Description page of Project Settings.


#include "MDGameMode.h"

AMDGameMode::AMDGameMode()
{
	CharacterStatTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Assets/Data/CharacterStat"));
}

void AMDGameMode::BeginPlay()
{
	Super::BeginPlay();
}

FCharacterStatData* AMDGameMode::GetCharacterStat(ECharacterType type)
{
	int IntType = (int)type;
	FName StringType = *FString::FromInt(IntType);

	return CharacterStatTable->FindRow<FCharacterStatData>(StringType, TEXT(""));
}
