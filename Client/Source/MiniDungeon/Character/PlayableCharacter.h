// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDCharacter.h"
#include "Protocol.pb.h"
#include "PlayableCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UAnimMontage;

struct FInputActionValue;

/**
 * 
 */
UCLASS()
class MINIDUNGEON_API APlayableCharacter : public AMDCharacter
{
	GENERATED_BODY()

	// input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TMap<EAttackType, UInputAction*> InputActionMap;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	
public:
	APlayableCharacter();

	bool IsMyPlayer() const;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// 네트워크 복제를 위한 변수
	UPROPERTY(Replicated)
	FVector ReplicatedLocation;

	UPROPERTY(Replicated)
	FRotator ReplicatedRotation;
	
private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);

	void OnQSkill(const FInputActionValue& Value);
	void OnESkill(const FInputActionValue& Value);
	void OnShift(const FInputActionValue& Value);
	//이게 필요한건지 고민중
	//void OnShiftEnd(const FInputActionValue& Value);

protected:
	const float MOVE_PACKET_SEND_DELAY = 0.2f;
	float MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

	// Cache
	FVector2D DesiredInput;
	FVector DesiredMoveDirection;
	float DesiredYaw;

	// Dirty Flag Test
	FVector2D LastDesiredInput;

	FVector TargetLocation;

public:
	Protocol::MoveState GetMoveState() { return PosInfo->state(); }
	void SetMoveState(Protocol::MoveState State);
	void SetPlayerInfo(const Protocol::PosInfo& Info);
	void SetDestInfo(const Protocol::PosInfo& Info);
	void Other_Attack(const Protocol::AttackInfo& Info);
	void PlayAttackMontage(EAttackType AttackType);

protected:
	class Protocol::PosInfo* PosInfo; // 현재 위치
	class Protocol::PosInfo* DestInfo; // 목적지

	UPROPERTY(Replicated)
	uint64 playerID;

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TObjectPtr<class UMDNetworkManager> Player;
};
