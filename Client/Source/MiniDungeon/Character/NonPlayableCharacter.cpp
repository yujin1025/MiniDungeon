// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayableCharacter.h"
#include "../AI/MDAIController.h"
#include <MDNetworkManager.h>
#include <Kismet/GameplayStatics.h>
#include <Game/MDPlayerController.h>
#include "Component/AttackComponent.h"
#include "Components/WidgetComponent.h"
#include "../Widget/HPBarWidget.h"

ANonPlayableCharacter::ANonPlayableCharacter()
{
	AIControllerClass = AMDAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	//AutoPossessPlayer = EAutoReceiveInput::Disabled; //빙의하지 않음

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	HPBarWidget->SetupAttachment(RootComponent);
}

bool ANonPlayableCharacter::Attack()
{
	if (IsDead)
		return false;

	ActionComponentMap[EAttackType::QSkillAttack]->PlayAttackMontage();
	CurrentActionCoolTimeMap[EAttackType::QSkillAttack] = CurrentDeltaTime + ActionCoolTimeMap[EAttackType::QSkillAttack];

	return true;
}

void ANonPlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();

	FVector Location = GetActorLocation();
	DestInfo->set_object_id(ObjectID);
	DestInfo->set_x(Location.X);
	DestInfo->set_y(Location.Y);
	DestInfo->set_z(Location.Z);
	DestInfo->set_yaw(GetControlRotation().Yaw);

	SetMoveState(Protocol::MOVE_STATE_IDLE);
	TargetLocation = GetActorLocation();

	if (HPBarWidget && HPBarWidget->GetWidget() != nullptr)
	{
		UHPBarWidget* HPWidget = Cast<UHPBarWidget>(HPBarWidget->GetWidget());
		if (HPWidget)
		{
			HPWidget->SetHealthComponent(HealthComponent);
		}
	}
}

void ANonPlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FVector Location = GetActorLocation();
	//PosInfo->set_object_id(ObjectID);
	//PosInfo->set_x(Location.X);
	//PosInfo->set_y(Location.Y);
	//PosInfo->set_z(Location.Z);
	//PosInfo->set_yaw(GetControlRotation().Yaw);

	//auto networkManager = GetGameInstance()->GetSubsystem<UMDNetworkManager>();
	//auto pc = Cast<AMDPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	//if (networkManager->isHost)
	//{
	//	// State 정보
	//	if (GetVelocity() == FVector::Zero())
	//		SetMoveState(Protocol::MOVE_STATE_IDLE);
	//	else
	//		SetMoveState(Protocol::MOVE_STATE_RUN);

	//	MovePacketSendTimer -= DeltaTime;

	//	if (MovePacketSendTimer <= 0)
	//	{
	//		// 이동 패킷 전송
	//		Protocol::CTS_MOVE MovePkt;
	//		Protocol::PosInfo* Info = new Protocol::PosInfo();
	//		Info->CopyFrom(*PosInfo);
	//		Info->set_state(GetMoveState());
	//		MovePkt.set_allocated_info(Info);

	//		// 패킷을 SendBufferRef로 직렬화
	//		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(MovePkt);
	//		if (IsValid(networkManager)) 
	//		{
	//			networkManager->SendPacket(sendBuffer);
	//		}
	//	}
	//}
	//else
	//{
	//	// Host의 게임 인스턴스가 아닌 경우 서버에서 받은 정보를 기반으로 이동
	//	const Protocol::MoveState State = PosInfo->state();
	//	SetMoveState(State);

	//	if (State == Protocol::MOVE_STATE_RUN)
	//	{
	//		SetActorRotation(FRotator(0, DestInfo->yaw(), 0));
	//		AddMovementInput(GetActorForwardVector());
	//	}
	//}
}
