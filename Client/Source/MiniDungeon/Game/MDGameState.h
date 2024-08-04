// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MDGameState.generated.h"

UENUM(BlueprintType)
enum class ESpawnType : uint8
{
	Khaimera,
	Grux,
};

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API AMDGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMDGameState();

private:
	int GruxCharacterId = 0;

	int StartKhaimeraCharacterId = 1;
	int CurrentKhaimeraCharacterId = 1;

private:
	UPROPERTY(EditAnywhere)
	TArray<FVector> KhaimeraSpawnPositions;

	UPROPERTY(EditAnywhere)
	FRotator KhaimeraSpawnRotation;

	UPROPERTY(EditAnywhere)
	FVector GruxSpawnPosition;

	UPROPERTY(EditAnywhere)
	FRotator GruxSpawnRotation;

private:
	UPROPERTY(EditAnywhere, Category = Character)
	TSubclassOf<class AMDCharacter> KhaimeraClass;

	UPROPERTY(EditAnywhere, Category = Character)
	TSubclassOf<class AMDCharacter> GruxClass;

private:
	TMap<int, float> MonsterHealthMap;
	int MaxFieldKhaimeraCount = 3;
	int CurrentSpawnKhaimeraPositionIndex = 0;

public:
	void OnChangedHealth(int ObjectID, float CurrentHealth);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	int GetDeadKhaimeraCount();
	bool IsDeadGrux();

private:
	void Spawn(ESpawnType SpawnType);
};
