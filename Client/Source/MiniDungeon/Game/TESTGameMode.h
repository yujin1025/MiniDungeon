// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TESTGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API ATESTGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATESTGameMode();
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};
