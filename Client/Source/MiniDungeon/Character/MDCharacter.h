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

	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Motion, meta = (AllowPrivateAccess = "true"))
	UHitDeadComponent* HitDeadComponent;

	UHealthComponent* HealthComponent;

protected:
	bool CheckCoolTime(EAttackType AttackType);

	FString GetEnumNameAsString(EAttackType EnumValue);

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:
	void Move(const FVector2D Value);
	void Look(const FVector2D Value);


// Attack Section
public:
	void SendAttackPacket(EAttackType AttackType);

	bool UseSkill(EAttackType AttackType);
	void OnFinishedSkillMotion(EAttackType AttackType);

	FOnUseSkill OnUseSkillDelegate;
	EAttackType ProgressingAttackType = EAttackType::Max;
	//FOnAttackEndDelegate OnAttackEnd;

protected:
	float CurrentDeltaTime = 0.0f;

public:
	virtual bool IsSatisfiedAttack(EAttackType AttackType);

	virtual void OnUseQSkill() {}
	virtual void OnUseESkill() {}
	virtual void OnUseShiftSkill() {}

	virtual void OnHit();
	virtual void OnDie();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action, meta = (AllowPrivateAccess = "true"))
	TMap<EAttackType, UAttackComponent*> ActionComponentMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Action, meta = (AllowPrivateAccess = "true"))
	TMap<EAttackType, float> ActionCoolTimeMap;

	TMap<EAttackType, float> CurrentActionCoolTimeMap;

public:
	bool IsPlayer();
	FVector GetLookVector(const AMDCharacter* Target) const;

	virtual FVector GetTargetPosition(ECollisionChannel Channel, float RayCastDistance, OUT bool& IsFoundTarget);
	void RotateToTarget(const AMDCharacter* Target, float RotationSpeed);
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

	UPROPERTY(Replicated)
	uint64 ObjectID;

protected:
	class Protocol::PosInfo* PosInfo; // 현재 위치
	class Protocol::PosInfo* DestInfo; // 목적지

	FVector TargetLocation;

	const float MOVE_PACKET_SEND_DELAY = 0.2f;
	float MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;
public:
	Protocol::MoveState GetMoveState() { return PosInfo->state(); }
	void SetMoveState(Protocol::MoveState State);
	void SetPosInfo(const Protocol::PosInfo& Info);
	void SetDestInfo(const Protocol::PosInfo& Info);

	uint64 GetObjectID() const { return ObjectID; }
	void SetObjectID(uint64 obj_id) { ObjectID = obj_id; }

	FRotator GetTargetRotation(FVector targetLocation);

	void StartSmoothRotation(FVector targetLocation, float speed);

	void UpdateSmoothRotationWithInterp();

	FTimerHandle SmoothRotationTimerHandle;

protected:
	float RotationSpeed = 0.f;
	FRotator TargetRotation;
};
