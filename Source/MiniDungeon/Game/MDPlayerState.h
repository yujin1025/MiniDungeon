// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MDPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChangedDelegate, int, float)
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
	void OnChangePlayerHealth(int objectId, float Amount);

private:
	float CurrentHP;
	
	// ����, HP, ���, ����, �̴ϸ� ��ġ ���� ����˴ϴ�.
};
