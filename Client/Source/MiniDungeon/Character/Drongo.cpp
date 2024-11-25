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

			EnhancedInputComponent->BindAction(InputActionMap[EAttackType::QSkillAttack], ETriggerEvent::Triggered, this, &ADrongo::OnQSkill);
			EnhancedInputComponent->BindAction(InputActionMap[EAttackType::ESkillAttack], ETriggerEvent::Triggered, this, &ADrongo::OnESkill);
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

    UseSkill(Protocol::PLAYER_TYPE_DRONGO, EAttackType::QSkillAttack);
}

void ADrongo::OnESkill(const FInputActionValue& Value)
{
	Super::OnESkill(Value);
    
    UseSkill(Protocol::PLAYER_TYPE_DRONGO, EAttackType::ESkillAttack);
}

void ADrongo::OnShift(const FInputActionValue& Value)
{
    Super::OnShift(Value);
	
    UseSkill(Protocol::PLAYER_TYPE_DRONGO, EAttackType::ShiftAttack);
}

void ADrongo::Fire(float DamageAmount)
{
    FVector CameraLocation;
    FRotator CameraRotation;
    Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector Start = CameraLocation;
    FVector End = Start + (CameraRotation.Vector() * 3500.0f); //길이 조절하기

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Pawn, CollisionParams))
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor)
        {
            ANonPlayableCharacter* DamagedCharacter = Cast<ANonPlayableCharacter>(HitActor);
            if (DamagedCharacter)
            {
                UHealthComponent* HealthComp = DamagedCharacter->FindComponentByClass<UHealthComponent>();
                if (HealthComp)
                {
                    HealthComp->ChangeHealth(this, -DamageAmount);
                }
            }
        }
    }
}
