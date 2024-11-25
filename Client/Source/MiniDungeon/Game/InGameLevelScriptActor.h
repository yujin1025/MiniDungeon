// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "InGameLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API AInGameLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	AInGameLevelScriptActor();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	TArray<AActor*> GetPlayerStarts() const { return PlayerStarts; }
	TArray<AActor*> GetEnemySpawns() const { return EnemySpawns; }
	AActor* GetBossSpawn() const { return BossSpawn; }
private:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Spawn", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> PlayerStarts;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Spawn", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> EnemySpawns;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Spawn", meta = (AllowPrivateAccess = "true"))
	AActor* BossSpawn;
};
