// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MDCharacter.generated.h"

class UAttackComponent;

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	QSkillAttack,
	ESkillAttack,
	ShiftAttack,
	Max,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUseSkill, EAttackType)

UCLASS()
class MINIDUNGEON_API AMDCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMDCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action, meta = (AllowPrivateAccess = "true"))
	TMap<EAttackType, UAttackComponent*> ActionComponentMap;

protected:
	FString GetEnumNameAsString(EAttackType EnumValue);

	virtual void BeginPlay() override;

public:
	void Move(const FVector2D Value);
	void Look(const FVector2D Value);


// Attack Section
public:
	bool UseSkill(EAttackType AttackType);
	void OnFinishedSkillMotion(EAttackType AttackType);

	FOnUseSkill OnUseSkillDelegate;
	EAttackType ProgressingAttackType = EAttackType::Max;

public:
	virtual void OnUseQSkill() {}
	virtual void OnUseESkill() {}
	virtual void OnUseShiftSkill() {}
};
