// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MiniDungeon.h"
#include "MDPlayerController.generated.h"

class AMDPlayerState;
class AMDCharacter;
/**
 * 
 */
UCLASS()
class MINIDUNGEON_API AMDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMDPlayerController();
	virtual void BeginDestroy() override;

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;

private:
	void OnPossessCharacter(AMDCharacter* aCharacter);
	AMDCharacter* OwnerCharacter;

	Protocol::PlayerInfo* PlayerInfo;

public:
	const Protocol::PlayerInfo* GetPlayerInfo() { return PlayerInfo; }
	void SetPlayerInfo(const Protocol::PlayerInfo& info);
public:
	AMDPlayerState* GetState();
	AMDCharacter* GetCharacter();
};
