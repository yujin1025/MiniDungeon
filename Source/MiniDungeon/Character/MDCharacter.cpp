// Fill out your copyright notice in the Description page of Project Settings.


#include "MDCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AMDCharacter::AMDCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
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

void AMDCharacter::UseSkill(EAttackType AttackType)
{
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
}



