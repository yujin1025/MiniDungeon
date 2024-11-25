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

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnMonsterHPChangedDelegate, int, float, float)
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
	UPROPERTY(EditAnywhere, Category = Character)
	TSubclassOf<class AMDCharacter> KhaimeraClass;

	UPROPERTY(EditAnywhere, Category = Character)
	TSubclassOf<class AMDCharacter> GruxClass;

public:
	void OnChangedHealth(int ObjectID, float CurrentHealth, float maxHP);
	FOnMonsterHPChangedDelegate OnMonsterHPChanged;

	/*TMap<int, float> MonsterHealthMap;
	TMap<int, float> GetMonsterHealthMap() const { return MonsterHealthMap; }
	void AddMonsterHealth(int objectID, float currentHealth);
	void RemoveMonsterHealth(int objectID) { MonsterHealthMap.Remove(objectID); }*/
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
