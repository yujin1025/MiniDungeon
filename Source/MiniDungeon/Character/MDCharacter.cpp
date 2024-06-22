// Fill out your copyright notice in the Description page of Project Settings.


#include "MDCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Component/AttackComponent.h"
#include "../Component/HealthComponent.h"
#include "../Component/HitDeadComponent.h"
#include "../Game/MDGameMode.h"

AMDCharacter::AMDCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HitDeadComponent = CreateDefaultSubobject<UHitDeadComponent>(TEXT("HitDeadComponent"));

	ActionComponentMap.Empty();
	for (int i = 0; i < (int)EAttackType::Max; i++)
	{
		auto AttackType = (EAttackType)i;

		FString AttackTypeStr = GetEnumNameAsString(AttackType);
		FString ComponentName = TEXT("AttackComponent [") + AttackTypeStr + "]";
		auto Component = CreateDefaultSubobject<UAttackComponent>((FName)*ComponentName);
		ActionComponentMap.Add(AttackType, Component);
	}

	switch (CharacterType)
	{
	case ECharacterType::Aurora:
		CharacterId = 0;
		break;
	case ECharacterType::Drongo:
		CharacterId = 1;
		break;
	case ECharacterType::Khaimera:
		CharacterId = 2;
		break;
	case ECharacterType::Grux:
		CharacterId = 3;
		break;
	}
}

FString AMDCharacter::GetEnumNameAsString(EAttackType EnumValue)
{
	switch (EnumValue)
	{
	case EAttackType::QSkillAttack:
		return "QSkillAttack";

	case EAttackType::ESkillAttack:
		return "ESkillAttack";

	case EAttackType::ShiftAttack:
		return "ShiftAttack";
	}

	return "";
}


void AMDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMDCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AMDCharacter::Move(const FVector2D Value)
{
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Value.Y);
		AddMovementInput(RightDirection, Value.X);
	}
}

void AMDCharacter::Look(const FVector2D Value)
{
	if (Controller != nullptr)
	{
		AddControllerYawInput(Value.X);
		AddControllerPitchInput(Value.Y);
	}
}

bool AMDCharacter::UseSkill(EAttackType AttackType)
{
	//if (ProgressingAttackType != EAttackType::Max)
	//	return false;

	if (ActionComponentMap.Contains(AttackType) == false)
		return false;

	ActionComponentMap[AttackType]->PlayAttackMontage();

	switch (AttackType)
	{
	case EAttackType::QSkillAttack:
		OnUseQSkill();
		break;
	case EAttackType::ESkillAttack:
		OnUseESkill();
		break;
	case EAttackType::ShiftAttack:
		OnUseShiftSkill();
		break;
	}

	//OnAttackEnd.Broadcast();
	ProgressingAttackType = AttackType;
	return true;
}

void AMDCharacter::OnFinishedSkillMotion(EAttackType AttackType)
{
	ProgressingAttackType = EAttackType::Max;
	if (AttackType != EAttackType::Max)
	{
		OnUseSkillDelegate.Broadcast(AttackType);
	}
}

void AMDCharacter::OnHit()
{
	if (HitDeadComponent)
	{
		HitDeadComponent->PlayHitMontage();
	}

	OnFinishedSkillMotion(ProgressingAttackType);
}

void AMDCharacter::OnDie()
{
	if (HitDeadComponent)
	{
		HitDeadComponent->PlayDeadMontage();
	}
}

bool AMDCharacter::IsPlayer()
{
	return Controller->IsPlayerController();
}

FVector AMDCharacter::GetLookVector(AMDCharacter*& Target) const
{
	return Target->GetActorLocation() - GetActorLocation();
}

FVector AMDCharacter::GetTargetPosition(ECollisionChannel Channel, float RayCastDistance, OUT bool& IsFoundTarget)
{
	FVector CameraLocation;
	FRotator CameraRotation;
	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector Start = CameraLocation;
	FVector End = Start + (CameraRotation.Vector() * RayCastDistance);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, Channel, CollisionParams))
	{
		IsFoundTarget = true;
		return HitResult.Location;
	}

	IsFoundTarget = false;
	return FVector::ZeroVector;
}

void AMDCharacter::RotateToTarget(AMDCharacter*& Target, float RotationSpeed)
{
	if (Target == nullptr)
		return;

	FVector LookVector = GetLookVector(Target);
	LookVector.Z = 0.f;

	FRotator TargetRotation = FRotationMatrix::MakeFromX(LookVector).Rotator();
	SetRotation(TargetRotation, RotationSpeed);
}

void AMDCharacter::SetRotation(FRotator Rotation, float RotationSpeed)
{
	FRotator TargetRotation = FMath::RInterpTo(GetActorRotation(), Rotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);
	SetActorRotation(TargetRotation);
}



