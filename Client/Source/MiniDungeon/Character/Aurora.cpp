// Fill out your copyright notice in the Description page of Project Settings.


#include "Aurora.h"
#include "MiniDungeon.h"
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AAurora::AAurora()
{
}

void AAurora::BeginPlay()
{
	MD_LOG(LogMDNetwork, Log, TEXT("Begin"));
	Super::BeginPlay();
	MD_LOG(LogMDNetwork, Log, TEXT("End"));
}

void AAurora::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
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

			EnhancedInputComponent->BindAction(InputActionMap[EAttackType::QSkillAttack], ETriggerEvent::Triggered, this, &AAurora::OnQSkill);
			EnhancedInputComponent->BindAction(InputActionMap[EAttackType::ESkillAttack], ETriggerEvent::Triggered, this, &AAurora::OnESkill);
			EnhancedInputComponent->BindAction(InputActionMap[EAttackType::ShiftAttack], ETriggerEvent::Started, this, &AAurora::OnShift);
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

void AAurora::OnQSkill(const FInputActionValue& Value)
{
	Super::OnQSkill(Value);

	UseSkill(Protocol::PLAYER_TYPE_AURORA, EAttackType::QSkillAttack);
}

void AAurora::OnESkill(const FInputActionValue& Value)
{
	Super::OnESkill(Value);

	UseSkill(Protocol::PLAYER_TYPE_AURORA, EAttackType::ESkillAttack);
}

void AAurora::OnShift(const FInputActionValue& Value)
{
	Super::OnShift(Value);

	UseSkill(Protocol::PLAYER_TYPE_AURORA, EAttackType::ShiftAttack);
}
