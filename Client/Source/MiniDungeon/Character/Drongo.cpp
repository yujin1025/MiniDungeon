// Fill out your copyright notice in the Description page of Project Settings.


#include "Drongo.h"
#include "NonPlayableCharacter.h"
#include "Component/HealthComponent.h"
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ADrongo::ADrongo()
{
}

void ADrongo::BeginPlay()
{
    Super::BeginPlay();
}

void ADrongo::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (IsMyPlayer() == true)
	{
		//Add Input Mapping Context
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}

		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayableCharacter::OnMove);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayableCharacter::OnMove);
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayableCharacter::OnLook);

			EnhancedInputComponent->BindAction(InputActionMap[EAttackType::QSkillAttack], ETriggerEvent::Started, this, &ADrongo::OnQSkill);
			EnhancedInputComponent->BindAction(InputActionMap[EAttackType::ESkillAttack], ETriggerEvent::Started, this, &ADrongo::OnESkill);
			EnhancedInputComponent->BindAction(InputActionMap[EAttackType::ShiftAttack], ETriggerEvent::Started, this, &ADrongo::OnShift);
			//EnhancedInputComponent->BindAction(InputActionMap[EAttackType::ShiftAttack], ETriggerEvent::Completed, this, &APlayableCharacter::OnShiftEnd);
		}
	}
	else
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayableCharacter::OnLook);
		}

		bUseControllerRotationYaw = false;   // 컨트롤러가 캐릭터 Yaw 회전 처리 안 함
		bUseControllerRotationPitch = false; // 컨트롤러가 캐릭터 Pitch 회전 처리 안 함
		bUseControllerRotationRoll = false;  // 컨트롤러가 캐릭터 Roll 회전 처리 안 함

		// 카메라 붐 설정
		CameraBoom->bUsePawnControlRotation = true; // 카메라 붐이 컨트롤러 회전을 따름

		// 카메라 설정
		FollowCamera->bUsePawnControlRotation = false; // 카메라는 붐의 회전을 따름
	}
}

void ADrongo::OnQSkill(const FInputActionValue& Value)
{
    Super::OnQSkill(Value);

	if(bCanUseQ == false)
	{
		return;
	}

	bCanUseQ = false;
	SendAttackPacket(Protocol::PLAYER_TYPE_DRONGO, EAttackType::QSkillAttack);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
		bCanUseQ = true;
		}, ActionCoolTimeMap[EAttackType::QSkillAttack], false);

    //UseSkill(Protocol::PLAYER_TYPE_DRONGO, EAttackType::QSkillAttack);
}

void ADrongo::OnESkill(const FInputActionValue& Value)
{
	Super::OnESkill(Value);

	if(bCanUseE == false)
	{
		return;
	}

	bCanUseE = false;
	SendAttackPacket(Protocol::PLAYER_TYPE_DRONGO, EAttackType::ESkillAttack);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
		bCanUseE = true;
		}, ActionCoolTimeMap[EAttackType::ESkillAttack], false);
    
    //UseSkill(Protocol::PLAYER_TYPE_DRONGO, EAttackType::ESkillAttack);
}

void ADrongo::OnShift(const FInputActionValue& Value)
{
    Super::OnShift(Value);
	
	if(bCanUseShift == false)
	{
		return;
	}

	bCanUseShift = false;
	SendAttackPacket(Protocol::PLAYER_TYPE_DRONGO, EAttackType::ShiftAttack);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
		bCanUseShift = true;
		}, ActionCoolTimeMap[EAttackType::ShiftAttack], false);
	
    //UseSkill(Protocol::PLAYER_TYPE_DRONGO, EAttackType::ShiftAttack);
}

void ADrongo::OnEndQSkill(const FInputActionValue& Value)
{
	Super::OnEndQSkill(Value);
}

void ADrongo::OnEndESkill(const FInputActionValue& Value)
{
	Super::OnEndESkill(Value);
}

void ADrongo::OnEndShift(const FInputActionValue& Value)
{
	Super::OnEndShift(Value);
}

