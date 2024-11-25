// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MDPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChangedDelegate, float, float)
/**
 * 
 */
UCLASS()
class MINIDUNGEON_API AMDPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMDPlayerState();

public:
	FOnHPChangedDelegate OnPlayerHPChanged;

public:
	void OnChangePlayerHealth(float Amount, float maxHP);

private:
	float CurrentHP;

	float MaxHP;
	
	// 레벨, HP, 목숨, 버프, 미니맵 위치 등이 저장됩니다.
};
