// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MDGameState.generated.h"


/**
 * 
 */
UCLASS()
class MINIDUNGEON_API AMDGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMDGameState();

public:
	void OnChangedHealth(int ObjectID, float Amount);
};
