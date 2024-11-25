// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Protocol.pb.h"
#include "Net/UnrealNetwork.h"
#include "../Network/MDNetworkManager.h"
#include "../Game/MDGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Component/AttackComponent.h"
#include "../Component/HealthComponent.h"

APlayableCharacter::APlayableCharacter()
{
	bReplicates = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 
	CameraBoom->bUsePawnControlRotation = true; 


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	InputActionMap.Add(EAttackType::QSkillAttack, nullptr);
	InputActionMap.Add(EAttackType::ESkillAttack, nullptr);
	InputActionMap.Add(EAttackType::ShiftAttack, nullptr);
}

bool APlayableCharacter::IsMyPlayer() const
{
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	if (networkManager)
	{
		if (networkManager->PlayerInfos.Contains(networkManager->PlayerID))
		{
			return networkManager->PlayerInfos[networkManager->PlayerID]->object_info().object_id() == ObjectID;
		}
	}

	return false;
}

void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();

	FVector Location = GetActorLocation();

	DestInfo->set_object_id(ObjectID);
	DestInfo->set_x(Location.X);
	DestInfo->set_y(Location.Y);
	DestInfo->set_z(Location.Z);
	DestInfo->set_yaw(GetControlRotation().Yaw);

	SetMoveState(Protocol::MOVE_STATE_IDLE);
	TargetLocation = GetActorLocation();
}


void APlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 위치를 PosInfo에 업데이트
	FVector Location = GetActorLocation();
	PosInfo->set_object_id(ObjectID);
	PosInfo->set_x(Location.X);
	PosInfo->set_y(Location.Y);
	PosInfo->set_z(Location.Z);
	PosInfo->set_yaw(GetControlRotation().Yaw);

	// 로컬 플레이어의 경우, 자신의 이동 정보를 서버로 전송
	if (IsMyPlayer())
	{
		// Send 판정
		bool ForceSendPacket = false;

		if (LastDesiredInput != DesiredInput)
		{
			ForceSendPacket = true;
			LastDesiredInput = DesiredInput;
		}

		// State 정보
		if (DesiredInput == FVector2D::Zero())
			SetMoveState(Protocol::MOVE_STATE_IDLE);
		else
			SetMoveState(Protocol::MOVE_STATE_RUN);

		MovePacketSendTimer -= DeltaTime;

		if (MovePacketSendTimer <= 0 || ForceSendPacket)
		{
			// 이동 패킷 전송
			Protocol::CTS_MOVE MovePkt;
			Protocol::PosInfo* Info = new Protocol::PosInfo();
			Info->CopyFrom(*PosInfo);
			Info->set_state(GetMoveState());
			MovePkt.set_allocated_info(Info);

			// 패킷을 SendBufferRef로 직렬화
			SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(MovePkt);
			auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
			if (networkManager) {
				networkManager->SendPacket(sendBuffer);
			}
		}
	}
	else
	{
		// 다른 플레이어의 경우 서버에서 받은 정보를 기반으로 이동
		const Protocol::MoveState State = PosInfo->state();
		SetMoveState(State);

		if (State == Protocol::MOVE_STATE_RUN)
		{
			SetActorRotation(FRotator(0, DestInfo->yaw(), 0));
			AddMovementInput(GetActorForwardVector());
		}
	}

}

void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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

			EnhancedInputComponent->BindAction(InputActionMap[EAttackType::QSkillAttack], ETriggerEvent::Triggered, this, &APlayableCharacter::OnQSkill);
			EnhancedInputComponent->BindAction(InputActionMap[EAttackType::ESkillAttack], ETriggerEvent::Triggered, this, &APlayableCharacter::OnESkill);
			EnhancedInputComponent->BindAction(InputActionMap[EAttackType::ShiftAttack], ETriggerEvent::Started, this, &APlayableCharacter::OnShift);
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

void APlayableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayableCharacter, ReplicatedLocation);
	DOREPLIFETIME(APlayableCharacter, ReplicatedRotation);
}

void APlayableCharacter::OnMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	//Move(MovementVector);

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		// Cache
		{
			DesiredInput = MovementVector;

			DesiredMoveDirection = FVector::ZeroVector;
			DesiredMoveDirection += ForwardDirection * MovementVector.Y;
			DesiredMoveDirection += RightDirection * MovementVector.X;
			DesiredMoveDirection.Normalize();

			const FVector Location = GetActorLocation();
			FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Location, Location + DesiredMoveDirection);
			DesiredYaw = Rotator.Yaw;
		}
	}
}

void APlayableCharacter::OnLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	Look(LookAxisVector);
}

void APlayableCharacter::OnQSkill(const FInputActionValue& Value)
{
	UseSkill(EAttackType::QSkillAttack);
}

void APlayableCharacter::OnESkill(const FInputActionValue& Value)
{
	UseSkill(EAttackType::ESkillAttack);
}

void APlayableCharacter::OnShift(const FInputActionValue& Value)
{
	UseSkill(EAttackType::ShiftAttack);
}


void APlayableCharacter::Other_Attack(const Protocol::AttackInfo& Info)
{
	UE_LOG(LogTemp, Warning, TEXT("Other_Hit"));
	// 1. 공격 타입에 따라 애니메이션을 재생합니다.
	EAttackType AttackType = static_cast<EAttackType>(Info.attack_type());
	PlayAttackMontage(AttackType);

	// 2. 피해를 적용합니다.
	float Damage = Info.damage();
	if (HealthComponent) 
	{
		//HealthComponent->ChangeHealth(this, -Damage);
	}
}

void APlayableCharacter::PlayAttackMontage(EAttackType AttackType)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ActionComponentMap.Contains(AttackType))
	{
		ActionComponentMap[AttackType]->PlayAttackMontage();
	}
}
