// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MDCharacter.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	QSkillAttack,
	ESkillAttack,
	ShiftAttack,
};



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


// Attack Section
public:
	virtual void UseSkill(EAttackType AttackType);

public:
	virtual void OnUseQSkill() {}
	virtual void OnUseESkill() {}
	virtual void OnUseShiftSkill() {}
};
