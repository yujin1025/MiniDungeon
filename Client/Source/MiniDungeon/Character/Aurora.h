// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayableCharacter.h"
#include "Aurora.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API AAurora : public APlayableCharacter
{
	GENERATED_BODY()
	
public:
	AAurora();

	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void OnQSkill(const FInputActionValue& Value) override;
	virtual void OnESkill(const FInputActionValue& Value) override;
	virtual void OnShift(const FInputActionValue& Value) override;

	virtual void OnEndQSkill(const FInputActionValue& Value) override;
	virtual void OnEndESkill(const FInputActionValue& Value) override;
	virtual void OnEndShift(const FInputActionValue& Value) override;

};
