// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MDCharacter.generated.h"

UCLASS()
class MINIDUNGEON_API AMDCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMDCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

public:
	void Move(const FVector2D Value);
	void Look(const FVector2D Value);
};
