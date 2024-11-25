// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayableCharacter.h"
#include "Drongo.generated.h"

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API ADrongo : public APlayableCharacter
{
	GENERATED_BODY()
public:
	ADrongo();

	virtual void BeginPlay() override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	virtual void OnQSkill(const FInputActionValue& Value) override;
	virtual void OnESkill(const FInputActionValue& Value) override;
	virtual void OnShift(const FInputActionValue& Value) override;

	void Fire(float DamageAmount);
};
