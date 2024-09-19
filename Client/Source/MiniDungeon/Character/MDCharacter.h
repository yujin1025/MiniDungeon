// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MiniDungeon.h"
#include "MDCharacter.generated.h"

class UAttackComponent;
class UHitDeadComponent;
class UHealthComponent;

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	QSkillAttack,
	ESkillAttack,
	ShiftAttack,
	Max,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUseSkill, EAttackType)
//DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class MINIDUNGEON_API AMDCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMDCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Motion, meta = (AllowPrivateAccess = "true"))
	UHitDeadComponent* HitDeadComponent;

	UHealthComponent* HealthComponent;

protected:
	bool CheckCoolTime(EAttackType AttackType);

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
	//FOnAttackEndDelegate OnAttackEnd;

private:
	float CurrentDeltaTime = 0.0f;

public:
	virtual bool IsSatisfiedAttack(EAttackType AttackType);

	virtual void OnUseQSkill() {}
	virtual void OnUseESkill() {}
	virtual void OnUseShiftSkill() {}

	virtual void OnHit();
	virtual void OnDie();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action, meta = (AllowPrivateAccess = "true"))
	TMap<EAttackType, UAttackComponent*> ActionComponentMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action, meta = (AllowPrivateAccess = "true"))
	TMap<EAttackType, float> ActionCoolTimeMap;

	TMap<EAttackType, float> CurrentActionCoolTimeMap;

public:
	bool IsPlayer();
	FVector GetLookVector(AMDCharacter*& Target) const;

	virtual FVector GetTargetPosition(ECollisionChannel Channel, float RayCastDistance, OUT bool& IsFoundTarget);
	void RotateToTarget(AMDCharacter*& Target, float RotationSpeed);
	void SetRotation(FRotator Rotation, float RotationSpeed);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Id, meta = (AllowPrivateAccess = "true"))
	int CharacterId;

	UFUNCTION(BlueprintCallable, Category = "Character")
	ECharacterType GetCharacterType() const { return CharacterType; }

	bool IsDead = false;

protected:
	// Character type
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	ECharacterType CharacterType;

};
