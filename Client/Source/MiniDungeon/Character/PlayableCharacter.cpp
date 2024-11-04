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

	TargetLocation = GetActorLocation();
	PosInfo = new Protocol::PosInfo();
	DestInfo = new Protocol::PosInfo();
}

bool APlayableCharacter::IsMyPlayer() const
{
	return IsLocallyControlled();
}

void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	UMDNetworkManager* NetworkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	if (NetworkManager)
	{
		playerID = NetworkManager->PlayerID; // PlayerID 가져오기
	}
}


void APlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			// 현재 위치를 PosInfo에 업데이트
			PosInfo->set_object_id(playerID);
			PosInfo->set_x(GetActorLocation().X);
			PosInfo->set_y(GetActorLocation().Y);
			PosInfo->set_z(GetActorLocation().Z);
			PosInfo->set_yaw(GetActorRotation().Yaw);

			// 이동 패킷 전송
			Protocol::CTS_MOVE MovePkt;
			Protocol::PosInfo* Info = new Protocol::PosInfo();
			Info->CopyFrom(*PosInfo);
			Info->set_state(GetMoveState());
			MovePkt.set_allocated_info(Info);

			UE_LOG(LogTemp, Log, TEXT("Sending CTS_MOVE Packet: ObjectID = %llu, X = %f, Y = %f, Z = %f, Yaw = %f, State = %d"),
				Info->object_id(),
				Info->x(),
				Info->y(),
				Info->z(),
				Info->yaw(),
				Info->state());

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
		FVector CurrentLocation = GetActorLocation();
		FVector SmoothLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 10.f);
		SetActorLocation(SmoothLocation);
	}

}

void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayableCharacter::OnLook);

		EnhancedInputComponent->BindAction(InputActionMap[EAttackType::QSkillAttack], ETriggerEvent::Triggered, this, &APlayableCharacter::OnQSkill);
		EnhancedInputComponent->BindAction(InputActionMap[EAttackType::ESkillAttack], ETriggerEvent::Triggered, this, &APlayableCharacter::OnESkill);
		EnhancedInputComponent->BindAction(InputActionMap[EAttackType::ShiftAttack], ETriggerEvent::Started, this, &APlayableCharacter::OnShift);
		//EnhancedInputComponent->BindAction(InputActionMap[EAttackType::ShiftAttack], ETriggerEvent::Completed, this, &APlayableCharacter::OnShiftEnd);
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
	Move(MovementVector);
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

void APlayableCharacter::SetMoveState(Protocol::MoveState State)
{
	if (PosInfo->state() == State)
		return;

	PosInfo->set_state(State);

	// TODO
}

void APlayableCharacter::SetPlayerInfo(const Protocol::PosInfo& Info)
{
	if (PosInfo->object_id() != 0)
	{
		assert(PosInfo->object_id() == Info.object_id());
	}

	PosInfo->CopyFrom(Info);

	FVector Location(Info.x(), Info.y(), Info.z());
	SetActorLocation(Location);
}

void APlayableCharacter::SetDestInfo(const Protocol::PosInfo& Info)
{
	if (PosInfo->object_id() != 0)
	{
		assert(PosInfo->object_id() == Info.object_id());
	}

	// Dest에 최종 상태 복사.
	DestInfo->CopyFrom(Info);

	// 상태만 바로 적용하자.
	SetMoveState(Info.state());
}
