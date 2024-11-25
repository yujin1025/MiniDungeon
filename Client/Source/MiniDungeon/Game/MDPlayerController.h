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
	void CheckGameState();

protected:
	virtual void Tick(float DeltaTime) override;


private:
	void OnPossessCharacter(AMDCharacter* aCharacter);
	AMDCharacter* OwnerCharacter;

	Protocol::PlayerInfo* PlayerInfo;

	bool bGameStateShown = false;

public:
	const Protocol::PlayerInfo* GetPlayerInfo() { return PlayerInfo; }
	void SetPlayerInfo(const Protocol::PlayerInfo& info);

public:
	AMDPlayerState* GetState();
	AMDCharacter* GetCharacter();

public:
	UPROPERTY()
	class UMDWidget* GameClearWidget;

	UPROPERTY()
	UMDWidget* GameOverWidget;

private:
	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UMDWidget> ClearWidgetClass;

	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UMDWidget> OverWidgetClass;

};
