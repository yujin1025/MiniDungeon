// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;

private:
	void OnPossessCharacter(AMDCharacter* aCharacter);
	AMDCharacter* OwnerCharacter;
	
public:
	AMDPlayerState* GetState();
	AMDCharacter* GetCharacter();
};
