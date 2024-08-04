// Fill out your copyright notice in the Description page of Project Settings.


#include "MDGameState.h"
#include "../Character/MDCharacter.h"
#include "../Character/Khaimera.h"
#include "../Character/Grux.h"

AMDGameState::AMDGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMDGameState::OnChangedHealth(int ObjectID, float CurrentHealth)
{
	if (MonsterHealthMap.Contains(ObjectID) == false)
		return;

	MonsterHealthMap[ObjectID] = CurrentHealth;
	/*
	if (Amount <= 0.0f)
	{
		AActor* Actor = GetOwner();
		if (Actor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Monster Destroyed!"));
		}
	}*/
}

void AMDGameState::BeginPlay()
{
	Super::BeginPlay();

	//Spawn(ESpawnType::Grux);

	CurrentKhaimeraCharacterId = StartKhaimeraCharacterId;

	for (int i = 0; i < MaxFieldKhaimeraCount; i++)
	{
		Spawn(ESpawnType::Khaimera);
	}
}

void AMDGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	int FieldKhaimeraCount = 0;

	for (auto& Pair : MonsterHealthMap)
	{
		// 센티넬이면
		if (Pair.Key >= StartKhaimeraCharacterId)
		{
			if (Pair.Value > 0)
			{
				FieldKhaimeraCount++;
			}
		}
	}

	if (FieldKhaimeraCount < MaxFieldKhaimeraCount)
	{
		int NumToSpawn = MaxFieldKhaimeraCount - FieldKhaimeraCount;
		for (int i = 0; i < NumToSpawn; i++)
		{
			Spawn(ESpawnType::Khaimera);
		}
	}
}

int AMDGameState::GetDeadKhaimeraCount()
{
	int DeadKhaimeraCount = 0;

	for (auto& Pair : MonsterHealthMap)
	{
		// Khaimera이면
		if (Pair.Key > -StartKhaimeraCharacterId)
		{
			if (Pair.Value <= 0)
			{
				DeadKhaimeraCount++;
			}
		}
	}

	return DeadKhaimeraCount;
}

bool AMDGameState::IsDeadGrux()
{
	return MonsterHealthMap[GruxCharacterId] <= 0.0f;
}

void AMDGameState::Spawn(ESpawnType SpawnType)
{
	AMDCharacter* Character;

	switch (SpawnType)
	{
	case ESpawnType::Khaimera:

		if (KhaimeraSpawnPositions.Num() > CurrentSpawnKhaimeraPositionIndex)
		{
			Character = GetWorld()->SpawnActor<AKhaimera>(KhaimeraClass, KhaimeraSpawnPositions[CurrentSpawnKhaimeraPositionIndex], KhaimeraSpawnRotation);
			Character->CharacterId = CurrentKhaimeraCharacterId;

			CurrentSpawnKhaimeraPositionIndex++;
			CurrentSpawnKhaimeraPositionIndex %= MaxFieldKhaimeraCount;

			MonsterHealthMap.Add(CurrentKhaimeraCharacterId, 30.0f);
			CurrentKhaimeraCharacterId++;
		}

		break;


	case ESpawnType::Grux:
		Character = GetWorld()->SpawnActor<AGrux>(GruxClass, GruxSpawnPosition, GruxSpawnRotation);
		Character->CharacterId = GruxCharacterId;

		MonsterHealthMap.Add(GruxCharacterId, 100.0f);

		break;
	}
}
